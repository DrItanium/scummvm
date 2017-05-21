/*
 * hanekawa
 * Copyright (c) 2017, Joshua Scoggins and Contributors
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HANEKAWA_SCRIPT_H
#define HANEKAWA_SCRIPT_H
#if __cplusplus < 201103L
#error "C++11 support required for hanekawa architecture!"
#endif

#include "common/scummsys.h"

namespace Hanekawa {

template<typename T, typename F, T bitmask, T shift>
constexpr F decodeBits(T input) noexcept {
	return static_cast<F>((input & bitmask) >> shift);
}

template<typename T, typename F, T bitmask, T shift>
constexpr T encodeBits(T input, F value) noexcept {
	return static_cast<T>((input & ~bitmask) | (static_cast<T>(value) << shift));
}

using Address = uint16;
using Word = int16;

class HanekawaEngine;

class Core {
public:
	Core(HanekawaEngine *vm);
	virtual ~Core();

private:
	HanekawaEngine *_vm;
};

} // End of namespace Hanekawa

#endif 
