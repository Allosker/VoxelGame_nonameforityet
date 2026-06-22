#include "entityChunkGrid.hpp"



void entities::EntityChunkGrid::update(entities::Player& player, const ItemTypeManager& itm) noexcept
{
	const types::loc player_loc = to_loc(player.getPosition());

	types::loc min{ player_loc - ChunkSettings::world_render_distance };
	types::loc max{ player_loc + ChunkSettings::world_render_distance };

	for (auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		const auto loc{ it->first };

		if (
			(loc.x < min.x || loc.x > max.x) ||
			(loc.y < min.y || loc.y > max.y) ||
			(loc.z < min.z || loc.z > max.z))
		{

			it = m_chunks.erase(it);
		}
		else
			++it;
	}

	update_items(player, itm);
}

void entities::EntityChunkGrid::update_items(entities::Player& player, const ItemTypeManager& itm) noexcept
{
	auto* current_chunk = chunk_at(player.getPosition());

	if (current_chunk)
		current_chunk->update_items(player, itm);
}

void entities::EntityChunkGrid::draw(const render::Shader& shader, const render::Texture& items_texture) noexcept
{
	for (auto& i : m_chunks)
		i.second.draw(shader, items_texture);
}

types::loc entities::EntityChunkGrid::to_loc(const types::pos& pos) noexcept
{
	return {
		static_cast<int32>(std::floor(pos.x / EntityChunk::g_size)),
		static_cast<int32>(std::floor(pos.y / EntityChunk::g_size)),
		static_cast<int32>(std::floor(pos.z / EntityChunk::g_size)) };
}

size_t entities::EntityChunkGrid::to_index(const types::loc& loc) noexcept
{
	/*const auto z_stride{ EntityChunk::g_size * chunks::Chunk::g_size };
	auto index = (size_t)(std::abs(std::floor(fpos.z) * z_stride + std::floor(fpos.y) * chunks::Chunk::g_size + std::floor(fpos.x)));*/

	return 0;
}
