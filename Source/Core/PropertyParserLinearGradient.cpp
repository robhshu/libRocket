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
#include "PropertyParserLinearGradient.h"

namespace Rocket {
namespace Core {

PropertyParserLinearGradient::PropertyParserLinearGradient()
{
}

PropertyParserLinearGradient::~PropertyParserLinearGradient()
{
}

// Called to parse a RCSS linear gradient declaration.
bool PropertyParserLinearGradient::ParseValue(Property& property, const String& value, const ParameterMap& parameters) const
{
	if (value.Empty())
		return false;

	LinearGradient linGrad;

	if (value.Substring(0, 15) == "linear-gradient")
	{
		Log::Message(Log::LT_INFO, "Parsing linear gradient '%s'", value.CString());

		StringList values;

		int find = value.Find("(") + 1;
		StringUtilities::ExpandString(values, value.Substring(find, value.RFind(")") - find), ',');

		if( values.size() >= 3 )
		{
			Property tmp;

			if( LocalNumberParser.ParseValue( tmp, values[0], parameters ) )
				linGrad.direction = tmp.Get<float>( );

			Log::Message(Log::LT_INFO, "Expected number '%s' %f", values[0].CString(), linGrad.direction);
			
			
			if( LocalColourParser.ParseValue( tmp, values[1], parameters ) )
				linGrad.top = tmp.Get< Colourb >( );

			Log::Message(Log::LT_INFO, "Expected colour '%s' %u", values[1].CString(), linGrad.top.red);

			if( LocalColourParser.ParseValue( tmp, values[2], parameters ) )
				linGrad.bottom = tmp.Get< Colourb >( );

			Log::Message(Log::LT_INFO, "Expected colour '%s' %u", values[2].CString(), linGrad.bottom.red);
		}
	}

	property.value = Variant(linGrad);
	property.unit = Property::LINEAR_GRADIENT;
	
	return true;
}

// Destroys the parser.
void PropertyParserLinearGradient::Release()
{
	delete this;
}

}
}
