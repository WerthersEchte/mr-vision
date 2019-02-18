#ifndef neural_h
#define neural_h

#include <c_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <io.h>

typedef struct model_t {
	TF_Graph* graph;
	TF_Session* session;
	TF_Status* status;

	TF_Output input, target, output;

	TF_Operation *init_op, *train_op, *save_op, *restore_op;
	TF_Output checkpoint_file;
} model_t;

int ModelCreate(model_t* model, const char* graph_def_filename);
void ModelDestroy(model_t* model);

int Okay(TF_Status* status);
TF_Buffer* ReadFile(const char* filename);
TF_Tensor* ScalarStringTensor(const char* data, TF_Status* status);
int DirectoryExists(const char* dirname);

#endif // !neural_h