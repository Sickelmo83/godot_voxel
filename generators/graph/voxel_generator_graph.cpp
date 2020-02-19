#include "voxel_generator_graph.h"

namespace {
VoxelGeneratorGraph::NodeTypeDB *g_node_type_db = nullptr;
}

VoxelGeneratorGraph::NodeTypeDB *VoxelGeneratorGraph::NodeTypeDB::get_singleton() {
	CRASH_COND(g_node_type_db == nullptr);
	return g_node_type_db;
}

void VoxelGeneratorGraph::NodeTypeDB::create_singleton() {
	CRASH_COND(g_node_type_db != nullptr);
	g_node_type_db = memnew(NodeTypeDB());
}

void VoxelGeneratorGraph::NodeTypeDB::destroy_singleton() {
	CRASH_COND(g_node_type_db == nullptr);
	memdelete(g_node_type_db);
	g_node_type_db = nullptr;
}

VoxelGeneratorGraph::NodeTypeDB::NodeTypeDB() {
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_CONSTANT];
		t.outputs.push_back(Port("Value", true));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_INPUT_X];
		t.outputs.push_back(Port("Value", true));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_INPUT_Y];
		t.outputs.push_back(Port("Value", true));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_INPUT_Z];
		t.outputs.push_back(Port("Value", true));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_OUTPUT_SDF];
		t.inputs.push_back(Port("Value", true));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_ADD];
		t.inputs.push_back(Port("A"));
		t.inputs.push_back(Port("B"));
		t.outputs.push_back(Port("Sum"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_SUBTRACT];
		t.inputs.push_back(Port("A"));
		t.inputs.push_back(Port("B"));
		t.outputs.push_back(Port("Sum"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_MULTIPLY];
		t.inputs.push_back(Port("A"));
		t.inputs.push_back(Port("B"));
		t.outputs.push_back(Port("Product"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_SINE];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_FLOOR];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_ABS];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_SQRT];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_DISTANCE_2D];
		t.inputs.push_back(Port("X0"));
		t.inputs.push_back(Port("Y0"));
		t.inputs.push_back(Port("X1"));
		t.inputs.push_back(Port("Y1"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_DISTANCE_3D];
		t.inputs.push_back(Port("X0"));
		t.inputs.push_back(Port("Y0"));
		t.inputs.push_back(Port("Y0"));
		t.inputs.push_back(Port("X1"));
		t.inputs.push_back(Port("Y1"));
		t.inputs.push_back(Port("Z1"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_CLAMP];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
		t.params.push_back(Param("Min", -1.0));
		t.params.push_back(Param("Max", 1.0));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_MIX];
		t.inputs.push_back(Port("A"));
		t.inputs.push_back(Port("B"));
		t.inputs.push_back(Port("Ratio"));
		t.outputs.push_back(Port("Result"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_REMAP];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
		t.params.push_back(Param("Min0", -1.0));
		t.params.push_back(Param("Max0", 1.0));
		t.params.push_back(Param("Min1", -1.0));
		t.params.push_back(Param("Max1", 1.0));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_CURVE];
		t.inputs.push_back(Port("X"));
		t.outputs.push_back(Port("Result"));
		t.params.push_back(Param("Curve"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_NOISE_2D];
		t.inputs.push_back(Port("X"));
		t.inputs.push_back(Port("Y"));
		t.outputs.push_back(Port("Result"));
		t.params.push_back(Param("Noise"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_NOISE_3D];
		t.inputs.push_back(Port("X"));
		t.inputs.push_back(Port("Y"));
		t.inputs.push_back(Port("Z"));
		t.outputs.push_back(Port("Result"));
		t.params.push_back(Param("Noise"));
	}
	{
		NodeType &t = types[VoxelGeneratorGraph::NODE_IMAGE_2D];
		t.inputs.push_back(Port("X"));
		t.inputs.push_back(Port("Y"));
		t.outputs.push_back(Port("Result"));
		t.params.push_back(Param("Image"));
	}
}

VoxelGeneratorGraph::VoxelGeneratorGraph() {

	typedef ProgramGraph::PortLocation PL;

	// Default
	uint32_t n_x = create_node(NODE_INPUT_X);
	uint32_t n_y = create_node(NODE_INPUT_Y);
	uint32_t n_z = create_node(NODE_INPUT_Y);
	uint32_t n_o = create_node(NODE_OUTPUT_SDF);
	uint32_t n_sin0 = create_node(NODE_SINE);
	uint32_t n_sin1 = create_node(NODE_SINE);
	uint32_t n_add = create_node(NODE_ADD);
	uint32_t n_mul0 = create_node(NODE_MULTIPLY);
	uint32_t n_mul1 = create_node(NODE_MULTIPLY);
	uint32_t n_mul2 = create_node(NODE_MULTIPLY);
	uint32_t n_c0 = create_node(NODE_CONSTANT);
	uint32_t n_c1 = create_node(NODE_CONSTANT);
	uint32_t n_sub = create_node(NODE_SUBTRACT);

	node_set_param(n_c0, 0, 1.0 / 20.0);
	node_set_param(n_c1, 0, 10.0);

	/*
	 *    X --- * --- sin           Z
	 *         /         \           \
	 *       1/20         + --- * --- - --- O
	 *         \         /     /
	 *    Y --- * --- sin    10.0
	*/

	node_connect(PL{ n_x, 0 }, PL{ n_mul0, 0 });
	node_connect(PL{ n_y, 0 }, PL{ n_mul1, 0 });
	node_connect(PL{ n_c0, 0 }, PL{ n_mul0, 1 });
	node_connect(PL{ n_c0, 0 }, PL{ n_mul1, 1 });
	node_connect(PL{ n_mul0, 0 }, PL{ n_sin0, 0 });
	node_connect(PL{ n_mul1, 0 }, PL{ n_sin1, 0 });
	node_connect(PL{ n_sin0, 0 }, PL{ n_add, 0 });
	node_connect(PL{ n_sin1, 0 }, PL{ n_add, 1 });
	node_connect(PL{ n_add, 0 }, PL{ n_mul2, 0 });
	node_connect(PL{ n_c1, 0 }, PL{ n_mul2, 1 });
	node_connect(PL{ n_z, 0 }, PL{ n_sub, 0 });
	node_connect(PL{ n_mul2, 0 }, PL{ n_sub, 1 });
	node_connect(PL{ n_sub, 0 }, PL{ n_o, 0 });

	_graph.debug_print_dot_file("voxel_graph_test.dot");
}

VoxelGeneratorGraph::~VoxelGeneratorGraph() {
	clear();
}

void VoxelGeneratorGraph::clear() {
	const uint32_t *key;
	while ((key = _nodes.next(key))) {
		Node *node = _nodes.get(*key);
		CRASH_COND(node == nullptr);
		memdelete(node);
	}
	_nodes.clear();
	_graph.clear();

	_program.clear();
	_memory.clear();
}

uint32_t VoxelGeneratorGraph::create_node(NodeTypeID type_id) {
	const NodeTypeDB::NodeType &type = NodeTypeDB::get_singleton()->types[type_id];

	ProgramGraph::Node *pg_node = _graph.create_node();
	pg_node->inputs.resize(type.inputs.size());
	pg_node->outputs.resize(type.outputs.size());

	Node *node = memnew(Node);
	node->params.resize(type.params.size());
	node->type = type_id;
	_nodes[pg_node->id] = node;

	return pg_node->id;
}

void VoxelGeneratorGraph::remove_node(uint32_t node_id) {
	_graph.remove_node(node_id);

	Node **pptr = _nodes.getptr(node_id);
	if (pptr != nullptr) {
		Node *node = *pptr;
		memdelete(node);
		_nodes.erase(node_id);
	}
}

void VoxelGeneratorGraph::node_connect(ProgramGraph::PortLocation src, ProgramGraph::PortLocation dst) {
	_graph.connect(src, dst);
}

void VoxelGeneratorGraph::node_set_param(uint32_t node_id, uint32_t param_index, Variant value) {
	Node **pptr = _nodes.getptr(node_id);
	ERR_FAIL_COND(pptr == nullptr);
	Node *node = *pptr;
	node->params[param_index] = value;
}

void VoxelGeneratorGraph::generate_block(VoxelBlockRequest &input) {

	VoxelBuffer &out_buffer = **input.voxel_buffer;

	const Vector3i bs = out_buffer.get_size();
	const VoxelBuffer::ChannelId channel = _channel;

	Vector3i rpos;
	Vector3i gpos;
	// Loads of possible optimization from there

	int stride = 1 << input.lod;
	for (rpos.z = 0, gpos.z = input.origin_in_voxels.z; rpos.z < bs.z; ++rpos.z, gpos.z += stride) {
		for (rpos.x = 0, gpos.x = input.origin_in_voxels.x; rpos.x < bs.x; ++rpos.x, gpos.x += stride) {
			for (rpos.y = 0, gpos.y = input.origin_in_voxels.y; rpos.y < bs.y; ++rpos.y, gpos.y += stride) {

				out_buffer.set_voxel_f(generate_single(gpos), rpos.x, rpos.y, rpos.z, channel);
			}
		}
	}

	out_buffer.compress_uniform_channels();
}

template <typename T>
inline void write_static(std::vector<uint8_t> &mem, uint32_t p, const T &v) {
	CRASH_COND(p + sizeof(T) >= mem.size());
	*(T *)(&mem[p]) = v;
}

template <typename T>
inline void append(std::vector<uint8_t> &mem, const T &v) {
	size_t p = mem.size();
	mem.resize(p + sizeof(T));
	*(T *)(&mem[p]) = v;
}

template <typename T>
inline void write_op(std::vector<uint8_t> &mem, uint8_t opid, T data) {
	uint32_t p = mem.size();
	mem.resize(p + 1 + sizeof(T));
	mem[p++] = opid;
	write_static(mem, p, data);
}

template <typename T>
inline const T &read(const std::vector<uint8_t> &mem, uint32_t &p) {
	CRASH_COND(p + sizeof(T) >= mem.size());
	const T *v = (const T *)&mem[p];
	p += sizeof(T);
	return *v;
}

inline float get_pixel_repeat(Image &im, int x, int y) {
	return im.get_pixel(wrap(x, im.get_width()), wrap(y, im.get_height())).r;
}

inline float squared(float x) {
	return x * x;
}

void VoxelGeneratorGraph::compile() {
	std::vector<uint32_t> order;
	_graph.evaluate(order);

	_program.clear();

	// Main inputs X, Y, Z
	_memory.resize(3);

	std::vector<uint8_t> &program = _program;
	const NodeTypeDB &type_db = *NodeTypeDB::get_singleton();
	HashMap<ProgramGraph::PortLocation, uint16_t, ProgramGraph::PortLocationHasher> output_port_addresses;
	bool has_output = false;

	for (size_t i = 0; i < order.size(); ++i) {
		const uint32_t node_id = order[i];
		const ProgramGraph::Node *pg_node = _graph.get_node(node_id);
		const Node *node = _nodes[node_id];
		const NodeTypeDB::NodeType &type = type_db.types[node->type];

		CRASH_COND(node == nullptr);
		CRASH_COND(pg_node->inputs.size() != type.inputs.size());
		CRASH_COND(pg_node->outputs.size() != type.outputs.size());

		switch (node->type) {
			case NODE_CONSTANT: {
				CRASH_COND(type.outputs.size() != 1);
				CRASH_COND(type.params.size() != 1);
				uint16_t a = _memory.size();
				_memory.push_back(node->params[0].operator float());
				output_port_addresses[ProgramGraph::PortLocation{ node_id, 0 }] = a;
			} break;

			case NODE_INPUT_X:
				output_port_addresses[ProgramGraph::PortLocation{ node_id, 0 }] = 0;
				break;

			case NODE_INPUT_Y:
				output_port_addresses[ProgramGraph::PortLocation{ node_id, 0 }] = 1;
				break;

			case NODE_INPUT_Z:
				output_port_addresses[ProgramGraph::PortLocation{ node_id, 0 }] = 2;
				break;

			case NODE_OUTPUT_SDF:
				// TODO Multiple outputs may be supported if we get branching
				CRASH_COND(has_output);
				has_output = true;
				break;

			default: {
				append(program, static_cast<uint8_t>(node->type));

				// Add inputs
				for (size_t j = 0; j < type.inputs.size(); ++j) {
					uint16_t a;

					if (pg_node->inputs[j].connections.size() == 0) {
						// No input, default it
						// TODO Take param value if specified
						a = _memory.size();
						_memory.push_back(0);

					} else {
						ProgramGraph::PortLocation src_port = pg_node->inputs[j].connections[0];
						const uint16_t *aptr = output_port_addresses.getptr(src_port);
						// Previous node ports must have been registered
						CRASH_COND(aptr == nullptr);
						a = *aptr;
					}

					append(program, a);
				}

				// Add outputs
				for (size_t j = 0; j < type.outputs.size(); ++j) {
					const uint16_t a = _memory.size();
					_memory.push_back(0);

					// This will be used by next nodes
					const ProgramGraph::PortLocation op{ node_id, static_cast<uint32_t>(j) };
					output_port_addresses[op] = a;

					append(program, a);
				}

				// Add special params
				switch (node->type) {

					case NODE_CURVE: {
						Ref<Curve> curve = node->params[0];
						CRASH_COND(curve.is_null());
						append(program, *curve);
					} break;

					case NODE_IMAGE_2D: {
						Ref<Image> im = node->params[0];
						CRASH_COND(im.is_null());
						append(program, *im);
					} break;

					case NODE_NOISE_2D:
					case NODE_NOISE_3D: {
						Ref<OpenSimplexNoise> noise = node->params[0];
						CRASH_COND(noise.is_null());
						append(program, *noise);
					} break;

					// TODO Worth it?
					case NODE_CLAMP:
						append(program, node->params[0].operator float());
						append(program, node->params[1].operator float());
						break;

					case NODE_REMAP: {
						float min0 = node->params[0].operator float();
						float max0 = node->params[1].operator float();
						float min1 = node->params[2].operator float();
						float max1 = node->params[3].operator float();
						append(program, -min0);
						append(program, Math::is_equal_approx(max0, min0) ? 99999.f : 1.f / (max0 - min0));
						append(program, min1);
						append(program, max1 - min1);
					} break;
				}

			} break;
		}
	}

	if (_memory.size() < 4) {
		// In case there is nothing
		_memory.resize(4, 0);
	}

	CRASH_COND(!has_output);
}

// The order of fields in the following structs matters.
// Inputs go first, then outputs, then params (if applicable at runtime).

struct PNodeBinop {
	uint16_t a_i0;
	uint16_t a_i1;
	uint16_t a_out;
};

struct PNodeMonoFunc {
	uint16_t a_in;
	uint16_t a_out;
};

struct PNodeDistance2D {
	uint16_t a_x0;
	uint16_t a_y0;
	uint16_t a_x1;
	uint16_t a_y1;
	uint16_t a_out;
};

struct PNodeDistance3D {
	uint16_t a_x0;
	uint16_t a_y0;
	uint16_t a_z0;
	uint16_t a_x1;
	uint16_t a_y1;
	uint16_t a_z1;
	uint16_t a_out;
};

struct PNodeClamp {
	uint16_t a_x;
	uint16_t a_out;
	float p_min;
	float p_max;
};

struct PNodeMix {
	uint16_t a_i0;
	uint16_t a_i1;
	uint16_t a_ratio;
	uint16_t a_out;
};

struct PNodeRemap {
	uint16_t a_x;
	uint16_t a_out;
	float p_c0;
	float p_m0;
	float p_c1;
	float p_m1;
};

struct PNodeCurve {
	uint16_t a_in;
	uint16_t a_out;
	Curve *p_curve;
};

struct PNodeNoise2D {
	uint16_t a_x;
	uint16_t a_y;
	uint16_t a_out;
	OpenSimplexNoise *p_noise;
};

struct PNodeNoise3D {
	uint16_t a_x;
	uint16_t a_y;
	uint16_t a_z;
	uint16_t a_out;
	OpenSimplexNoise *p_noise;
};

struct PNodeImage2D {
	uint16_t a_x;
	uint16_t a_y;
	uint16_t a_out;
	Image *p_image;
};

float VoxelGeneratorGraph::generate_single(const Vector3i &position) {
	// This part must be optimized for speed

	std::vector<float> &memory = _memory;
	memory[0] = position.x;
	memory[1] = position.y;
	memory[2] = position.z;

	uint32_t pc = 0;
	while (pc < _program.size()) {

		const uint8_t opid = _program[pc++];

		switch (opid) {
			case NODE_CONSTANT:
			case NODE_INPUT_X:
			case NODE_INPUT_Y:
			case NODE_INPUT_Z:
			case NODE_OUTPUT_SDF:
				// Not part of the runtime
				CRASH_NOW();
				break;

			case NODE_ADD: {
				const PNodeBinop &n = read<PNodeBinop>(_program, pc);
				memory[n.a_out] = memory[n.a_i0] + memory[n.a_i1];
			} break;

			case NODE_SUBTRACT: {
				const PNodeBinop &n = read<PNodeBinop>(_program, pc);
				memory[n.a_out] = memory[n.a_i0] - memory[n.a_i1];
			} break;

			case NODE_MULTIPLY: {
				const PNodeBinop &n = read<PNodeBinop>(_program, pc);
				memory[n.a_out] = memory[n.a_i0] * memory[n.a_i1];
			} break;

			case NODE_SINE: {
				const PNodeMonoFunc &n = read<PNodeMonoFunc>(_program, pc);
				memory[n.a_out] = Math::sin(Math_PI * memory[n.a_in]);
			} break;

			case NODE_FLOOR: {
				const PNodeMonoFunc &n = read<PNodeMonoFunc>(_program, pc);
				memory[n.a_out] = Math::floor(memory[n.a_in]);
			} break;

			case NODE_ABS: {
				const PNodeMonoFunc &n = read<PNodeMonoFunc>(_program, pc);
				memory[n.a_out] = Math::abs(memory[n.a_in]);
			} break;

			case NODE_SQRT: {
				const PNodeMonoFunc &n = read<PNodeMonoFunc>(_program, pc);
				memory[n.a_out] = Math::sqrt(memory[n.a_in]);
			} break;

			case NODE_DISTANCE_2D: {
				const PNodeDistance2D &n = read<PNodeDistance2D>(_program, pc);
				memory[n.a_out] = Math::sqrt(squared(memory[n.a_x1] - memory[n.a_x0]) +
											 squared(memory[n.a_y1] - memory[n.a_y0]));
			} break;

			case NODE_DISTANCE_3D: {
				const PNodeDistance3D &n = read<PNodeDistance3D>(_program, pc);
				memory[n.a_out] = Math::sqrt(squared(memory[n.a_x1] - memory[n.a_x0]) +
											 squared(memory[n.a_y1] - memory[n.a_y0]) +
											 squared(memory[n.a_z1] - memory[n.a_z0]));
			} break;

			case NODE_MIX: {
				const PNodeMix &n = read<PNodeMix>(_program, pc);
				memory[n.a_out] = Math::lerp(memory[n.a_i0], memory[n.a_i1], memory[n.a_ratio]);
			} break;

			case NODE_CLAMP: {
				const PNodeClamp &n = read<PNodeClamp>(_program, pc);
				memory[n.a_out] = clamp(memory[n.a_x], memory[n.p_min], memory[n.p_max]);
			} break;

			case NODE_REMAP: {
				const PNodeRemap &n = read<PNodeRemap>(_program, pc);
				memory[n.a_out] = ((memory[n.a_x] - n.p_c0) * n.p_m0) * n.p_m1 + n.p_c1;
			} break;

			case NODE_CURVE: {
				const PNodeCurve &n = read<PNodeCurve>(_program, pc);
				memory[n.a_out] = n.p_curve->interpolate_baked(memory[n.a_in]);
			} break;

			case NODE_NOISE_2D: {
				const PNodeNoise2D &n = read<PNodeNoise2D>(_program, pc);
				memory[n.a_out] = n.p_noise->get_noise_2d(memory[n.a_x], memory[n.a_y]);
			} break;

			case NODE_NOISE_3D: {
				const PNodeNoise3D &n = read<PNodeNoise3D>(_program, pc);
				memory[n.a_out] = n.p_noise->get_noise_3d(memory[n.a_x], memory[n.a_y], memory[n.a_z]);
			} break;

			case NODE_IMAGE_2D: {
				const PNodeImage2D &n = read<PNodeImage2D>(_program, pc);
				// TODO Not great, but in Godot 4.0 we won't need to lock anymore. Otherwise, need to do it in a pre-run and post-run
				n.p_image->lock();
				memory[n.a_out] = get_pixel_repeat(*n.p_image, memory[n.a_x], memory[n.a_y]);
				n.p_image->unlock();
			} break;

			default:
				CRASH_NOW();
				break;
		}
	}

	return memory.back() * _iso_scale;
}

void VoxelGeneratorGraph::_bind_methods() {

	ClassDB::bind_method(D_METHOD("clear"), &VoxelGeneratorGraph::clear);
}