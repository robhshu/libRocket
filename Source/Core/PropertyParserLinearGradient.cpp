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

	Gradientb *linGrad = new Gradientb();

	if (value.Substring(0, 15) == "linear-gradient")
	{
		StringList values;

		int find = value.Find("(") + 1;
		StringUtilities::ExpandString(values, value.Substring(find, value.RFind(")") - find), ',');

		if( values.size() >= 2 )
		{
			Property stop_val, stop_perc;
			int vIdx = 0;

			if( LocalNumberParser.ParseValue( stop_val, values[vIdx], parameters ) )
			{
				float fVal = 0.0f;

				if( !AngleAsDegrees( stop_val, fVal ) )
					Log::Message(Log::LT_WARNING, "Unsupported linear gradient orientation");

				linGrad->SetDirection( fVal );
				vIdx++;
			}

			String colour_string;
			String fixed_colour;

			for( ; vIdx < values.size(); )
			{
				stop_perc.unit = Property::UNKNOWN;

				if( FixupColour( values, vIdx, fixed_colour ) && PreParseColourStopValue( stop_val, stop_perc, fixed_colour, parameters ) )
				{
					Log::Message(Log::LT_INFO, "Source colour %s", fixed_colour.CString());

					const Gradientb::StopColourType colRes = stop_val.Get<Gradientb::StopColourType >( );

					float fAngle = 0;

					if( stop_perc.unit == Property::PERCENT )
						fAngle = stop_perc.Get<float >();

					linGrad->AddStop( colRes, fAngle );
				}
				else break;
			}

			if( linGrad->GetAllStops().size() < 2 )
				Log::Message(Log::LT_WARNING, "Too few colour stops for linear-gradient");
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

// Convert gradient angle to degrees
bool PropertyParserLinearGradient::AngleAsDegrees( const Property& angProp, float &outAngle ) const
{
	const float fVal = angProp.Get<float>( );
	
	switch( angProp.unit )
	{
		case Property::DEG:
			outAngle = fVal;
		break;
		case Property::GRAD:
			outAngle = Math::GradiansToDegrees(fVal);
		break;
		case Property::RAD:
			outAngle = Math::RadiansToDegrees(fVal);
		break;
		case Property::TURN:
			outAngle = Math::TurnsToDegrees(fVal);
		break;
		default:
			outAngle = 0.0f;
			return false;
	}

	return true;		
}

// Identifies colour definitions which also use bracket parameters
bool PropertyParserLinearGradient::FixupColour( const StringList &rStrs, int &rIdx, String &refString ) const
{
	const String &value = rStrs[rIdx++];
	
	String nvalue = value;

	if( value.Substring(0, 3) == "rgb" )
	{
		if( rIdx + 2 <= rStrs.size() )
		{
			// Join the RGB colour back
			nvalue += "," + rStrs[rIdx] + "," + rStrs[rIdx+1];
			rIdx += 2;
		}
		else
		{
			return false;
		}
	}

	refString = nvalue;
	
	return true;
}

// Parses colour and stop value from string
bool PropertyParserLinearGradient::PreParseColourStopValue(Property& colProp, Property& stopProp, const String& value, const ParameterMap& parameters) const
{
	String str_colour, str_stop;
	bool bHasStop = false;

	int spacePos = value.RFind(" ");
	if( spacePos > 0 )
	{
		str_colour = value.Substring(0, spacePos);
		str_stop = value.Substring(spacePos);
		bHasStop = true;
	}
	else
	{
		str_colour = value;
	}


	if( LocalColourParser.ParseValue( colProp, str_colour, parameters ) )
	{
		if( bHasStop )
			return LocalNumberParser.ParseValue( stopProp, str_stop, parameters );

		return true;
	}

	return false;
}

}
}
