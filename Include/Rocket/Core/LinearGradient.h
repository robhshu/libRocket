/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef ROCKETCORELINEARGRADIENT_H
#define ROCKETCORELINEARGRADIENT_H

#include <Rocket/Core/Header.h>
#include <vector>

namespace Rocket {
namespace Core {

/**
	Templated class for storage of linear gradients

	@author Robert H
 */

template < class ColourType >
class LinearGradient
{
public:
	typedef typename ColourType StopColourType;
	typedef typename std::vector<ColourType > Stops;

	/// Default constructor.
	LinearGradient( );

	/// Copy constructor.
	LinearGradient( const LinearGradient& rhs );

	/// Set current direction.
	inline void SetDirection( float fDir ) { direction = fDir; }

	/// Add colour stop to the list.
	void AddStop( const ColourType &colour, float ROCKET_UNUSED(fStopPos) = 0.0f );

	/// Get the current direction.
	inline float GetDirection( ) const { return direction; }

	/// Get the entire list of colour stops.
	inline const Stops &GetAllStops( ) const;

private:
	float direction;
	Stops colour_list;
	
};

}
}

namespace Rocket {
namespace Core {

#include "LinearGradient.inl"

}
}

#endif
