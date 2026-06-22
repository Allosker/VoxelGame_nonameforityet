#pragma once // chunk.hpp
// MIT
// Allosker ------------------------------
// =========+
// Basic chunk class responsible for everything related to gameplay that happens within it.
// ---------------------------------------

#include "utilities/types.hpp"
#include "data/voxel.hpp"

#include <vector>

namespace chunks
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

		types::pos getPos() const noexcept { return static_cast<types::pos>(m_pos); }

		types::pos getOppositeCorner() const noexcept { return static_cast<types::pos>(m_pos + static_cast<int64>(32)); }

		const std::vector<Data::Voxel>& getVoxels() const noexcept { return m_voxels; }

		types::block_index getVoxelIndex(const types::pos& pos) const;


		// = Predicates

		const bool is_within_chunk(const types::pos& point) const noexcept;

		const bool is_empty_at(const types::block_index& index) const noexcept { return block_at(index).id == 0; }

		bool isEmpty() const noexcept { return m_empty; }


		// = Mutators

		Data::Voxel& block_at(const types::loc& loc) 
		{ 
			return block_at(loc.x + loc.y * g_size + loc.z * g_size * g_size );
		}
		Data::Voxel& block_at(types::block_index index)
		{
			if (m_empty)
				return m_voxels.front();

			return m_voxels.at(index);
		}

		Data::Voxel* block_at_ptr(const types::loc& loc) noexcept;
		Data::Voxel* block_at_ptr(types::block_index index) noexcept;

		const Data::Voxel* block_at_ptr(const types::loc& loc) const noexcept;
		const Data::Voxel* block_at_ptr(types::block_index index) const noexcept;

		const Data::Voxel& block_at(types::block_index index) const
		{ 
			if (m_empty)
				return m_voxels.front();

			return m_voxels.at(index);
		}
		const Data::Voxel& block_at(const types::loc& loc) const { return block_at(loc.x + loc.y * g_size + loc.z * g_size * g_size); }

		const types::type_id block_id_at(const types::loc& loc) const noexcept
		{
			const Data::Voxel* block = block_at_ptr(loc.x + loc.y * g_size + loc.z * g_size * g_size);

			if (block)
				return block->id;

			return {};
		}


		// = Actors

		void make_empty() noexcept;
		void make_full() noexcept;


	public:

		bool generated_sunlight{ false };

	private:


		std::vector<Data::Voxel> m_voxels;

		types::loc m_pos{};

		bool m_empty{ false };
		

	public:

		static constexpr std::uint8_t g_size{ 32 };
		static constexpr std::uint32_t g_maxSize{ g_size * g_size * g_size };

	};

}