#include "text.hpp"



// =====================
// Construction/Destruction
// =====================

Render::GUI::Text::Text(const Font& font, std::string_view str, const vec3f& pos, const vec3f& ori)
	: Transform3D(pos, ori), p_font{ &font }, m_text{ str }
{
	// create GPU data

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Data::Vertex2D), std::bit_cast<void*>(offsetof(Data::Vertex2D, pos)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Data::Vertex2D), std::bit_cast<void*>(offsetof(Data::Vertex2D, uv)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	update_buffer();
}


// =====================
// Actors
// =====================

void Render::GUI::Text::draw(const Render::Shader& shader)
{
	if (!p_font)
		throw std::runtime_error("ERROR::TEXT::DRAWING: Cannot draw text that isn't bound to any font");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader.use();
	p_font->bind();

	shader.setValue("textColor", m_color);
	shader.setValue("model", getTransformation());

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_size_data);
}

/*private*/ void Render::GUI::Text::update_buffer()
{
	if (!p_font)
		throw std::runtime_error("ERROR::TEXT::BUFFER_UPDATE: Cannot update buffer because no font is bound to the current text");

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	std::vector<Data::Vertex2D> data{};

	vec2f tPos{};

	std::string::const_iterator c{};
	for (c = m_text.begin(); c != m_text.end(); c++)
	{
		Character ch{ p_font->getCharacter(*c) };

		vec2f pos{ tPos.x + ch.bearing.x * m_scale_text, tPos.y - (ch.size.y - ch.bearing.y) * m_scale_text };
		vec2f size{ ch.size.x * m_scale_text, ch.size.y * m_scale_text };

		data.insert(data.end(),
			{
				Data::Vertex2D
				{ vec2f{pos.x, pos.y + size.y},						static_cast<vec2f>(ch.pos)													},
				{ pos,												static_cast<vec2f>(vec2iu {ch.pos.x, ch.pos.y + ch.size.y})					},
				{ vec2f{pos.x + size.x, pos.y},						static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y + ch.size.y})	},

				{ vec2f{pos.x,				pos.y + size.y},		static_cast<vec2f>(vec2iu {ch.pos.x,				ch.pos.y})				},
				{ vec2f{pos.x + size.x,		pos.y},					static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y + ch.size.y})	},
				{ vec2f{pos.x + size.x,		pos.y + size.y},		static_cast<vec2f>(vec2iu {ch.pos.x + ch.size.x,	ch.pos.y})				},
			});

		tPos.x += (ch.advance >> 6) * m_scale_text;
	}

	m_size_data = data.size();

	glBufferData(GL_ARRAY_BUFFER, sizeof(Data::Vertex2D) * data.size(), data.data(), GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
