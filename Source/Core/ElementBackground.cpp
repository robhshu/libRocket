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
#include "ElementBackground.h"
#include <Rocket/Core/Element.h>
#include <Rocket/Core/GeometryUtilities.h>
#include <Rocket/Core/Property.h>

namespace Rocket {
namespace Core {

ElementBackground::ElementBackground(Element* _element) : geometry(_element)
{
	element = _element;
	background_dirty = true;
}

ElementBackground::~ElementBackground()
{
}

// Renders the element's background, if it has one.
void ElementBackground::RenderBackground()
{
	if (background_dirty)
	{
		background_dirty = false;
		GenerateBackground();
	}

	geometry.Render(element->GetAbsoluteOffset(Box::PADDING));
}

// Marks the background geometry as dirty.
void ElementBackground::DirtyBackground()
{
	background_dirty = true;
}

// Generates the background geometry for the element.
void ElementBackground::GenerateBackground()
{
	const Property* bgProp = element->GetProperty(BACKGROUND_IMAGE);
	
	if( bgProp->unit == Property::LINEAR_GRADIENT )
	{
		const Gradientb * bgGrad = bgProp->value.Get<Gradientb* >( );
		
		int num_boxes = 0;

		for (int i = 0; i < element->GetNumBoxes(); ++i)
		{
			const Box& box = element->GetBox(i);
			Vector2f size = box.GetSize(Box::PADDING);
			if (size.x > 0 && size.y > 0)
				num_boxes++;
		}

		std::vector< Vertex >& vertices = geometry.GetVertices();
		std::vector< int >& indices = geometry.GetIndices();

		const unsigned int stopCount = bgGrad->GetAllStops().size();

		if( stopCount > 1 )
		{
			int index_offset = 0;
			vertices.resize(4 * num_boxes * (stopCount-1) );
			indices.resize(6 * num_boxes * (stopCount-1) );

			Gradientb::Stops gradColours;
			GenerateColourList(gradColours, bgGrad);

			const int gradUnit = ConvertGradientUnit(bgGrad);
			const bool bHorizontal = ( gradUnit == 1 || gradUnit == 3 );

			if (num_boxes > 0 && stopCount > 0 )
			{
				Vertex* raw_vertices = &vertices[0];
				int* raw_indices = &indices[0];

				for (int i = 0; i < element->GetNumBoxes(); ++i)
					GenerateBackgroundGrad(raw_vertices, raw_indices, index_offset, element->GetBox(i), gradColours, bHorizontal);
			}
		}
		geometry.Release();

	}
	else
	{
		// Fetch the new colour for the background. If the colour is transparent, then we don't render any background.
		Colourb colour = element->GetProperty(BACKGROUND_COLOR)->value.Get< Colourb >();
		if (colour.alpha <= 0)
		{
			geometry.GetVertices().clear();
			geometry.GetIndices().clear();
			geometry.Release();

			return;
		}

		// Work out how many boxes we need to generate geometry for.
		int num_boxes = 0;

		for (int i = 0; i < element->GetNumBoxes(); ++i)
		{
			const Box& box = element->GetBox(i);
			Vector2f size = box.GetSize(Box::PADDING);
			if (size.x > 0 && size.y > 0)
				num_boxes++;
		}

		std::vector< Vertex >& vertices = geometry.GetVertices();
		std::vector< int >& indices = geometry.GetIndices();

		int index_offset = 0;
		vertices.resize(4 * num_boxes);
		indices.resize(6 * num_boxes);

		if (num_boxes > 0)
		{
			Vertex* raw_vertices = &vertices[0];
			int* raw_indices = &indices[0];

			for (int i = 0; i < element->GetNumBoxes(); ++i)
				GenerateBackground(raw_vertices, raw_indices, index_offset, element->GetBox(i), colour);
		}

		geometry.Release();
	}
}

// Generates the background geometry for a single box.
void ElementBackground::GenerateBackground(Vertex*& vertices, int*& indices, int& index_offset, const Box& box, const Colourb& colour)
{
	Vector2f padded_size = box.GetSize(Box::PADDING);
	if (padded_size.x <= 0 ||
		padded_size.y <= 0)
		return;

	GeometryUtilities::GenerateQuad(vertices, indices, box.GetOffset(), padded_size, colour, index_offset);

	vertices += 4;
	indices += 6;
	index_offset += 4;
}

// Generates the background geometry for a single box with a gradient.
void ElementBackground::GenerateBackgroundGrad(Vertex*& vertices, int*& indices, int& index_offset, const Box& box, const Gradientb::Stops& lgrad_colours, bool bHoriontal)
{
	Vector2f padded_size = box.GetSize(Box::PADDING);
	if (padded_size.x <= 0 ||
		padded_size.y <= 0)
		return;

	if( lgrad_colours.size() >= 2 )
	{
		std::vector< Gradientb::StopColourType > colours;
		colours.resize(4);
	
		Vector2f offset = box.GetOffset();
		Vector2f step(0, 0);

		if( bHoriontal )
		{
			const float fStep_x = padded_size.x / static_cast<float >(lgrad_colours.size()-1);
			step.x = fStep_x;
			padded_size.x = fStep_x;
		}
		else
		{
			const float fStep_y = padded_size.y / static_cast<float >(lgrad_colours.size()-1);
			step.y = fStep_y;
			padded_size.y = fStep_y;
		}

		for( Gradientb::Stops::size_type i=0; i<lgrad_colours.size()-1; i++ )
		{
			if( bHoriontal )
			{
				colours[0] = colours[3] = lgrad_colours[i];
				colours[1] = colours[2] = lgrad_colours[i+1];
			}
			else
			{
				colours[0] = colours[1] = lgrad_colours[i];
				colours[2] = colours[3] = lgrad_colours[i+1];
			}

			GeometryUtilities::GenerateQuadGrad(vertices, indices, offset, padded_size, &colours[0], index_offset);

			vertices += 4;
			indices += 6;
			index_offset += 4;

			offset += step;
		}
	}
}

// Placeholder function
int ElementBackground::ConvertGradientUnit( const Gradientb* lgrad_desc )
{
	const float angle = lgrad_desc->GetDirection();

	if( angle == 90 ) return 1;
	if( angle == 180 ) return 2;
	if( angle == 270 ) return 3;

	return 0;
}

void ElementBackground::GenerateColourList( Gradientb::Stops &refColours, const Gradientb *lgrad_desc )
{
	// Make copy
	refColours = lgrad_desc->GetAllStops();

	const int gradUnit = ConvertGradientUnit( lgrad_desc );

	// 'to left' or 'to bottom'
	if( gradUnit == 0 || gradUnit == 3 )
		std::reverse(refColours.begin(), refColours.end());
}

}
}
