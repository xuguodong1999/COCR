// a libtorch impl of https://github.com/masashitsubaki/molecularGNN_smiles.git
#include <torch/torch.h>

class MolecularGraphNeuralNetworkImpl : public torch::nn::Module {
    torch::nn::Embedding embed_fingerprint;
    torch::nn::ModuleList W_fingerprint, W_output;
    torch::nn::Linear W_property;

    void throwError() const {
        throw std::runtime_error("unknown task=" + std::to_string(static_cast<int>(mTask)));
    }

public:
    enum class TaskType {
        REGRESSION, CLASSIFICATION
    } mTask;

    MolecularGraphNeuralNetworkImpl(
            const int64_t &N_fingerprints,
            const int64_t &dim,
            const int64_t &layer_hidden,
            const int64_t &layer_output,
            const TaskType &task = TaskType::REGRESSION) :
            mTask(task),
            embed_fingerprint(torch::nn::Embedding(N_fingerprints, dim)),
            W_property(torch::nn::Linear(dim, TaskType::CLASSIFICATION == task ? 2 : 1)) {
        for (int64_t i = 0; i < layer_hidden; i++) {
            W_fingerprint->push_back(torch::nn::Linear(dim, dim));
        }
        for (int64_t i = 0; i < layer_output; i++) {
            W_output->push_back(torch::nn::Linear(dim, dim));
        }
    }

    torch::Tensor pad(const torch::Tensor &matrices, const float &value) {
        return {};
    }

    torch::Tensor gnn(const torch::Tensor &inputs) {
        return {};
    }

    torch::Tensor mlp(const torch::Tensor &vectors) {
        return {};
    }

    torch::Tensor forward(const torch::Tensor &data_batch, bool train) {
        // equal to data_batch[:-1]
        auto inputs = data_batch.narrow(0, 0, data_batch.size(0) - 1);
        auto ground_truth = data_batch[-1];
        if (train) {
            auto molecular_vectors = gnn(inputs);
            auto predicted_scores = mlp(molecular_vectors);
            torch::Tensor loss;
            if (TaskType::CLASSIFICATION == mTask) {
                loss = torch::cross_entropy_loss(predicted_scores, ground_truth);
            } else if (TaskType::REGRESSION == mTask) {
                loss = torch::mse_loss(predicted_scores, ground_truth);
            } else {
                throwError();
            }
            return loss;
        } else {
            // in the lifecycle of this var, codes behave like with torch.no_grad(): ...
            torch::NoGradGuard no_grad;
            auto molecular_vectors = gnn(inputs);
            auto predicted_scores = mlp(molecular_vectors);
            // use torch::split to recover them
            return torch::cat({predicted_scores, ground_truth});
        }
    }
};

TORCH_MODULE(MolecularGraphNeuralNetwork);

int main(int argc, char **argv) {
    return 0;
}