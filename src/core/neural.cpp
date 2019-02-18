#include "src/core/neural.h"
#include <iostream>
#include <fstream>
#include <vector>

int ModelCreate(model_t* model, const char* graph_def_filename) {
	model->status = TF_NewStatus();
	model->graph = TF_NewGraph();

	{
		// Create the session.
		TF_SessionOptions* opts = TF_NewSessionOptions();
		model->session = TF_NewSession(model->graph, opts, model->status);
		TF_DeleteSessionOptions(opts);
		if (!Okay(model->status)) return 0;
	}

	TF_Graph* g = model->graph;

	{
		// Import the graph.
		TF_Buffer* graph_def = ReadFile(graph_def_filename);
		if (graph_def == NULL) return 0;
		printf("Read GraphDef of %zu bytes\n", graph_def->length);
		TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();
		TF_GraphImportGraphDef(g, graph_def, opts, model->status);
		TF_DeleteImportGraphDefOptions(opts);
		TF_DeleteBuffer(graph_def);
		if (!Okay(model->status)) return 0;
	}

	// Handles to the interesting operations in the graph.
	model->input.oper = TF_GraphOperationByName(g, "input/X");
	model->input.index = 0;
	model->output.oper = TF_GraphOperationByName(g, "FullyConnected_1/Softmax");
	model->output.index = 0;
	return 1;
}

void ModelDestroy(model_t* model) {
	TF_DeleteSession(model->session, model->status);
	Okay(model->status);
	TF_DeleteGraph(model->graph);
	TF_DeleteStatus(model->status);
}

int Okay(TF_Status* status) {
	if (TF_GetCode(status) != TF_OK) {
		fprintf(stderr, "ERROR: %s\n", TF_Message(status));
		return 0;
	}
	return 1;
}

static std::vector<char> ReadAllBytes(char const* filename)
{
	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<char>  result(pos);

	ifs.seekg(0, std::ios::beg);
	ifs.read(&result[0], pos);

	return result;
}

TF_Buffer* ReadFile(const char* filename) {
	int fd = open(filename, 0);
	if (fd < 0) {
		perror("failed to open file: ");
		return NULL;
	}
	auto bytes = ReadAllBytes(filename);
	TF_Buffer* ret = TF_NewBufferFromString(bytes.data(), bytes.size());
	return ret;
}

TF_Tensor* ScalarStringTensor(const char* str, TF_Status* status) {
	size_t nbytes = 8 + TF_StringEncodedSize(strlen(str));
	TF_Tensor* t = TF_AllocateTensor(TF_STRING, NULL, 0, nbytes);
	char* data = (char*)TF_TensorData(t);
	memset(data, 0, 8);  // 8-byte offset of first string.
	TF_StringEncode(str, strlen(str), data + 8, nbytes - 8, status);
	return t;
}

int DirectoryExists(const char* dirname) {
	struct stat buf;
	return stat(dirname, &buf) == 0;
}