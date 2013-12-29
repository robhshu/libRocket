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

	LinearGradient *linGrad = new LinearGradient();

	if (value.Substring(0, 15) == "linear-gradient")
	{
		StringList values;

		int find = value.Find("(") + 1;
		StringUtilities::ExpandString(values, value.Substring(find, value.RFind(")") - find), ',');

		if( values.size() >= 2 )
		{
			Property tmp;
			int vIdx = 0;

			// May start with angle OR begin with a colour list

			if( LocalNumberParser.ParseValue( tmp, values[vIdx], parameters ) )
			{
				linGrad->angle_deg = tmp.Get<float>( );
				vIdx++;
			}

			// TODO: Stop value between 0 and 100%

			Rocket::Core::String colour_string;

			for( ; vIdx < values.size(); )
			{
				if( LocalColourParser.ParseValue( tmp, values[vIdx], parameters ) )
				{
					const Colourb colRes = tmp.Get< Colourb >( );

					Rocket::Core::TypeConverter< Rocket::Core::Colourb, Rocket::Core::String >::Convert(colRes, colour_string);
					Log::Message(Log::LT_INFO, "Converted colour '%s'=>%s", values[vIdx].CString(), colour_string.CString());

					linGrad->AddColour( colRes );
					vIdx++;
				}
				else
					break;
			}
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
