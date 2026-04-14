#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic chunk class responsible for everything related to gameplay that happens within it.
// ---------------------------------------

#include "uHeaders/types.hpp"
#include "rendering/world_managing/data/basic/voxel.hpp"

#include <vector>

#include "mpml/vectors/special_overloads/print_vectors.hpp"
namespace GameWorld::Voxels
{

	class Chunk
	{
	public: 

		// = Destruction/Initialization

		explicit Chunk(const types::loc& pos) noexcept;

		explicit Chunk() noexcept = default;

		Chunk(Chunk&&) noexcept = default;
		Chunk& operator=(Chunk&&) noexcept = default;

		Chunk(const Chunk&) = delete;
		Chunk& operator=(const Chunk&) = delete;


		// = Getters
			
		const std::vector<Render::Data::Voxel>& getVoxelData() const noexcept { return m_voxels; };

		std::vector<Render::Data::Voxel>& getVoxelData() noexcept { return m_voxels; }

		types::pos getPos() const noexcept { return static_cast<types::pos>(m_pos); }

		types::pos getOppositeCorner() const noexcept { return static_cast<types::pos>(m_pos + static_cast<int64>(32)); }


		// = Predicates

		const bool isWithinChunk(const types::pos& point) const noexcept;

		const bool is_empty_at(const types::chunk_index& index) const noexcept { return m_voxels.at(index).id == 0; }


		// = Mutators

		Render::Data::Voxel& block_at(const types::loc& loc) { return block_at(loc.x + loc.y * g_size + loc.z * g_size * g_size ); }
		Render::Data::Voxel& block_at(const types::chunk_index& index) { return m_voxels.at(index); }

		Render::Data::Voxel* block_at_ptr(const types::loc& loc) noexcept;
		Render::Data::Voxel* block_at_ptr(const types::chunk_index& index) noexcept;

		const Render::Data::Voxel* block_at_ptr(const types::loc& loc) const noexcept;
		const Render::Data::Voxel* block_at_ptr(const types::chunk_index& index) const noexcept;

		const Render::Data::Voxel& block_at(const types::chunk_index& index) const { return m_voxels.at(index); }
		const Render::Data::Voxel& block_at(const types::loc& loc) const { return block_at(loc.x + loc.y * g_size + loc.z * g_size * g_size); }
		
	private:

	private:


		std::vector<Render::Data::Voxel> m_voxels;

		types::loc m_pos{};
		

	public:

		static constexpr std::uint8_t g_size{ 32 };
		static constexpr std::uint32_t g_maxSize{ g_size * g_size * g_size };

	};

}