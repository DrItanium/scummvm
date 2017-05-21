   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*            CLIPS Version 6.40  01/06/16             */
   /*                                                     */
   /*            DEFFACTS CONSTRUCTS-TO-C MODULE          */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the constructs-to-c feature for the   */
/*    deffacts construct.                                    */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Dantes                                      */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.30: Added support for path name argument to        */
/*            constructs-to-c.                               */
/*                                                           */
/*            Removed conditional code for unsupported       */
/*            compilers/operating systems (IBM_MCW,          */
/*            MAC_MCW, and IBM_TBC).                         */
/*                                                           */
/*            Added const qualifiers to remove C++           */
/*            deprecation warnings.                          */
/*                                                           */
/*************************************************************/

#include "setup.h"

#if DEFFACTS_CONSTRUCT && CONSTRUCT_COMPILER && (! RUN_TIME)

#include <stdio.h>

#include "conscomp.h"
#include "dffctdef.h"
#include "envrnmnt.h"

#include "dffctcmp.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static bool                    ConstructToCode(void *,const char *,const char *,char *,int,FILE *,int,int);
   static void                    DeffactsToCode(void *,FILE *,struct deffacts *,
                                                 int,int,int);
   static void                    DeffactsModuleToCode(void *,FILE *,struct defmodule *,int,int,int);
   static void                    CloseDeffactsFiles(void *,FILE *,FILE *,int);
   static void                    BeforeDeffactsToCode(void *);

/*************************************************************/
/* DeffactsCompilerSetup: Initializes the deffacts construct */
/*    for use with the constructs-to-c command.              */
/*************************************************************/
void DeffactsCompilerSetup(
  void *theEnv)
  {
   DeffactsData(theEnv)->DeffactsCodeItem = 
      AddCodeGeneratorItem(theEnv,"deffacts",0,BeforeDeffactsToCode,
                           NULL,ConstructToCode,2);
  }

/*************************************************************/
/* BeforeDeffactsToCode: Assigns each deffacts a unique ID   */
/*   which will be used for pointer references when the data */
/*   structures are written to a file as C code              */
/*************************************************************/
static void BeforeDeffactsToCode(
  void *theEnv)
  {
   MarkConstructBsaveIDs(theEnv,DeffactsData(theEnv)->DeffactsModuleIndex);
  }

/**********************************************************/
/* ConstructToCode: Produces deffacts code for a run-time */
/*   module created using the constructs-to-c function.   */
/**********************************************************/
static bool ConstructToCode(
  void *theEnv,
  const char *fileName,
  const char *pathName,
  char *fileNameBuffer,
  int fileID,
  FILE *headerFP,
  int imageID,
  int maxIndices)
  {
   int fileCount = 1;
   struct defmodule *theModule;
   struct deffacts *theDeffacts;
   int moduleCount = 0, moduleArrayCount = 0, moduleArrayVersion = 1;
   int deffactsArrayCount = 0, deffactsArrayVersion = 1;
   FILE *moduleFile = NULL, *deffactsFile = NULL;

   /*===============================================*/
   /* Include the appropriate deffacts header file. */
   /*===============================================*/

   fprintf(headerFP,"#include \"dffctdef.h\"\n");

   /*=================================================================*/
   /* Loop through all the modules and all the deffacts writing their */
   /* C code representation to the file as they are traversed.        */
   /*=================================================================*/

   for (theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,NULL);
        theModule != NULL;
        theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,theModule))
     {
      EnvSetCurrentModule(theEnv,(void *) theModule);

      moduleFile = OpenFileIfNeeded(theEnv,moduleFile,fileName,pathName,fileNameBuffer,fileID,imageID,&fileCount,
                                    moduleArrayVersion,headerFP,
                                    "struct deffactsModule",ModulePrefix(DeffactsData(theEnv)->DeffactsCodeItem),
                                    false,NULL);

      if (moduleFile == NULL)
        {
         CloseDeffactsFiles(theEnv,moduleFile,deffactsFile,maxIndices);
         return(false);
        }

      DeffactsModuleToCode(theEnv,moduleFile,theModule,imageID,maxIndices,moduleCount);
      moduleFile = CloseFileIfNeeded(theEnv,moduleFile,&moduleArrayCount,&moduleArrayVersion,
                                     maxIndices,NULL,NULL);

      /*===================================================*/
      /* Loop through each of the deffacts in this module. */
      /*===================================================*/

      for (theDeffacts = (struct deffacts *) EnvGetNextDeffacts(theEnv,NULL);
           theDeffacts != NULL;
           theDeffacts = (struct deffacts *) EnvGetNextDeffacts(theEnv,theDeffacts))
        {
         deffactsFile = OpenFileIfNeeded(theEnv,deffactsFile,fileName,pathName,fileNameBuffer,fileID,imageID,&fileCount,
                                         deffactsArrayVersion,headerFP,
                                         "struct deffacts",ConstructPrefix(DeffactsData(theEnv)->DeffactsCodeItem),
                                         false,NULL);
         if (deffactsFile == NULL)
           {
            CloseDeffactsFiles(theEnv,moduleFile,deffactsFile,maxIndices);
            return(false);
           }

         DeffactsToCode(theEnv,deffactsFile,theDeffacts,imageID,maxIndices,moduleCount);
         deffactsArrayCount++;
         deffactsFile = CloseFileIfNeeded(theEnv,deffactsFile,&deffactsArrayCount,
                                          &deffactsArrayVersion,maxIndices,NULL,NULL);
        }

      moduleCount++;
      moduleArrayCount++;
     }

   CloseDeffactsFiles(theEnv,moduleFile,deffactsFile,maxIndices);

   return(true);
  }

/*********************************************************/
/* CloseDeffactsFiles: Closes all of the C files created */
/*   for deffacts. Called when an error occurs or when   */
/*   the deffacts have all been written to the files.    */
/*********************************************************/
static void CloseDeffactsFiles(
  void *theEnv,
  FILE *moduleFile,
  FILE *deffactsFile,
  int maxIndices)
  {
   int count = maxIndices;
   int arrayVersion = 0;

   if (deffactsFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,deffactsFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

   if (moduleFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,moduleFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }
  }

/**********************************************************/
/* DeffactsModuleToCode: Writes the C code representation */
/*   of a single deffacts module to the specified file.   */
/**********************************************************/
static void DeffactsModuleToCode(
  void *theEnv,
  FILE *theFile,
  struct defmodule *theModule,
  int imageID,
  int maxIndices,
  int moduleCount)
  {
#if MAC_XCD
#pragma unused(moduleCount)
#endif
   
   fprintf(theFile,"{");

   ConstructModuleToCode(theEnv,theFile,theModule,imageID,maxIndices,
                                  DeffactsData(theEnv)->DeffactsModuleIndex,
                                  ConstructPrefix(DeffactsData(theEnv)->DeffactsCodeItem));

   fprintf(theFile,"}");
  }

/*********************************************************/
/* DeffactsToCode: Writes the C code representation of a */
/*   single deffacts construct to the specified file.    */
/*********************************************************/
static void DeffactsToCode(
  void *theEnv,
  FILE *theFile,
  struct deffacts *theDeffacts,
  int imageID,
  int maxIndices,
  int moduleCount)
  {
   /*=================*/
   /* Deffacts Header */
   /*=================*/

   fprintf(theFile,"{");

   ConstructHeaderToCode(theEnv,theFile,&theDeffacts->header,imageID,maxIndices,
                         moduleCount,ModulePrefix(DeffactsData(theEnv)->DeffactsCodeItem),
                         ConstructPrefix(DeffactsData(theEnv)->DeffactsCodeItem));

   fprintf(theFile,",");

   /*=============*/
   /* Assert List */
   /*=============*/

   ExpressionToCode(theEnv,theFile,theDeffacts->assertList);
   fprintf(theFile,"}");
  }

/**************************************************************/
/* DeffactsCModuleReference: Writes the C code representation */
/*   of a reference to a deffacts module data structure.      */
/**************************************************************/
void DeffactsCModuleReference(
  void *theEnv,
  FILE *theFile,
  int count,
  int imageID,
  int maxIndices)
  {
   fprintf(theFile,"MIHS &%s%d_%d[%d]",
                      ModulePrefix(DeffactsData(theEnv)->DeffactsCodeItem),
                      imageID,
                      (count / maxIndices) + 1,
                      (count % maxIndices));
  }

#endif /* DEFFACTS_CONSTRUCT && CONSTRUCT_COMPILER && (! RUN_TIME) */


