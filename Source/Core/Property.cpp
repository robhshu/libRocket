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

#include "precompiled.h"
#include <Rocket/Core/Property.h>
#include <Rocket/Core/PropertyDefinition.h>

namespace Rocket {
namespace Core {

Property::Property() : unit(UNKNOWN), specificity(-1)
{
	definition = NULL;
	parser_index = -1;
}

Property::~Property()
{
}

String Property::ToString() const
{
	if (definition == NULL)
		return value.Get< String >();

	String string;
	definition->GetValue(string, *this);
	return string;
}

template< typename T >
const T InterpolateType( const T& a, const T&b, const float w )
{
	if( a == b )
	{
		return a;
	}
	else
	{
		return Math::Lerp<T>(a, b, w);
	}
}

template< >
const Colourf InterpolateType<Colourf >( const Colourf &a, const Colourf &b, const float w )
{
	Colourf result;
	result.red = InterpolateType(a.red, b.red, w);
	result.green = InterpolateType(a.green, b.green, w);
	result.blue = InterpolateType(a.blue, b.blue, w);
	result.alpha = InterpolateType(a.alpha, b.alpha, w);
	return result;
}

template< >
const Colourb InterpolateType<Colourb >( const Colourb &a, const Colourb &b, const float w )
{
	Colourf val_a;
	Colourf val_b;
	TypeConverter<Colourb, Colourf >::Convert( a, val_a );
	TypeConverter<Colourb, Colourf >::Convert( b, val_b );

	Colourf val_col = InterpolateType(val_a, val_b, w);
	
	Colourb result;
	TypeConverter<Colourf, Colourb >::Convert( val_col, result );
	return result;
}

Property Property::InterpolateNumber( const Property &a, const Property &b, const float w )
{
	const float val_a = a.value.Get<float >( );
	const float val_b = b.value.Get<float >( );

	const float lerp_val = InterpolateType( val_a, val_b, w );

	Property result(lerp_val, a.unit);
	result.source = "@keyframes";
	result.source_line_number = a.source_line_number;
	return result;
}

Property Property::InterpolateColour( const Property &a, const Property &b, const float w )
{
	const Colourb val_a = a.value.Get<Colourb >( );
	const Colourb val_b = b.value.Get<Colourb >( );

	const Colourb lerp_val = InterpolateType( val_a, val_b, w );

	Property result(lerp_val, Property::COLOUR);
	result.source = "@keyframes";
	result.source_line_number = a.source_line_number;
	return result;
}

Property Property::InterpolateLinearGradient( const Property &a, const Property &b, const float w )
{
	const Gradientb* val_a = a.value.Get<Gradientb* >();
	const Gradientb* val_b = b.value.Get<Gradientb* >();

	// Dirty debug check to see if these are valid pointers
	ROCKET_ASSERT( val_a != NULL && val_b != NULL );
	
	const Gradientb::Stops &val_list_a = val_a->GetAllStops();
	const Gradientb::Stops &val_list_b = val_b->GetAllStops();

	// Dirty debug check to see if these are both equal in size
	ROCKET_ASSERT( val_list_a.size() == val_list_b.size() );

	size_t i = 0;
	Gradientb* pLg = new Gradientb();

	while( i < val_list_b.size() )
	{
		const Gradientb::StopColourType lerp_val_i = InterpolateType<Gradientb::StopColourType >( val_list_a[i], val_list_b[i], w );
		pLg->AddStop( lerp_val_i );
		i++;
	}

	const float val_angle_a = val_a->GetDirection();
	const float val_angle_b = val_b->GetDirection();

	const float lerp_angle = InterpolateType( val_angle_a, val_angle_b, w );
	pLg->SetDirection( lerp_angle );

	Property result(pLg, Property::LINEAR_GRADIENT);
	result.source = "@keyframes";
	result.source_line_number = a.source_line_number;
	return result;
}

}
}
