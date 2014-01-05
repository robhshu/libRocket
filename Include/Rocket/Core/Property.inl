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

// Interpolate between values of the same type
template< typename T >
Property Property::Interpolate( const Property &a, const Property &b, const float w )
{
	const T& val_a = a.value.Get<T>( );
	const T& val_b = b.value.Get<T>( );
	const T lerp_val = Math::Lerp<T>(val_a, val_b, w);

	Property result(lerp_val, a.unit);
	result.source = "@keyframes";
	result.source_line_number = a.source_line_number;
	return result;
}

// Colourb specialization, converting from byte to float so can can interpolate
template <>
Property Property::Interpolate<Colourb >( const Property &a, const Property &b, const float w )
{
	Colourf val_a;
	Colourf val_b;
	TypeConverter<Colourb, Colourf >::Convert( a.value.Get<Colourb >(), val_a );
	TypeConverter<Colourb, Colourf >::Convert( b.value.Get<Colourb >(), val_b );

	Colourf val_col;
	val_col.red = Math::Lerp(val_a.red, val_b.red, w);
	val_col.green = Math::Lerp(val_a.green, val_b.green, w);
	val_col.blue = Math::Lerp(val_a.blue, val_b.blue, w);
	val_col.alpha = Math::Lerp(val_a.alpha, val_b.alpha, w);
	
	Colourb lerp_val;
	TypeConverter<Colourf, Colourb >::Convert( val_col, lerp_val );

	Property result(lerp_val, Property::COLOUR);
	result.source = "@keyframes";
	result.source_line_number = a.source_line_number;
	return result;
}
