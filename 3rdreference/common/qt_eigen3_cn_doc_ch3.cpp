/**
 * https://github.com/qixianyu-buaa/EigenChineseDocument/tree/master/Eigen/Chapter3_SparseLinearAlgebra
 */
#include <iostream>
// The Eigen/Dense header file defines all member functions for the MatrixXd type and related types
#include <Eigen/Dense>
//All classes and functions defined in this header file (and other Eigen header files) are in the Eigen namespace.

#define LOG()  printf("\n=========  %s() Output is  ========= \n",__FUNCTION__);

#include <Eigen/Sparse>
#include <vector>
#include <QImage>

typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double
typedef Eigen::Triplet<double> T;

void insertCoefficient(int id, int i, int j, double w, std::vector<T> &coeffs,
                       Eigen::VectorXd &b, const Eigen::VectorXd &boundary) {
    int n = int(boundary.size());
    int id1 = i + j * n;
    if (i == -1 || i == n)
        b(id) -= w * boundary(j); // constrained coefficient
    else if (j == -1 || j == n)
        b(id) -= w * boundary(i); // constrained coefficient
    else
        coeffs.push_back(T(id, id1, w)); // unknown coefficient
}

void buildProblem(std::vector<T> &coefficients, Eigen::VectorXd &b, int n) {
    b.setZero();
    Eigen::ArrayXd boundary = Eigen::ArrayXd::LinSpaced(n, 0, M_PI).sin().pow(2);
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            int id = i + j * n;
            insertCoefficient(id, i - 1, j, -1, coefficients, b, boundary);
            insertCoefficient(id, i + 1, j, -1, coefficients, b, boundary);
            insertCoefficient(id, i, j - 1, -1, coefficients, b, boundary);
            insertCoefficient(id, i, j + 1, -1, coefficients, b, boundary);
            insertCoefficient(id, i, j, 4, coefficients, b, boundary);
        }
    }
}

void saveAsBitmap(const Eigen::VectorXd &x, int n, const char *filename) {
    Eigen::Array<unsigned char, Eigen::Dynamic, Eigen::Dynamic> bits = (x * 255).cast<unsigned char>();
    QImage img(bits.data(), n, n, QImage::Format_Indexed8);
    img.setColorCount(256);
    for (int i = 0; i < 256; i++)
        img.setColor(i, qRgb(i, i, i));
    img.save(filename);
}

using namespace Eigen;
using namespace std;

namespace Chapter3_SparseLinearAlgebra {
    namespace Section1_SparseMatrixManipulations {
        // 稀疏矩阵格式
        // 在许多应用中，矩阵其只有少量非零系数，这样的矩阵称为稀疏矩阵(Sparse Matrix)。
        // 在这种情况下，采用稀疏矩阵的存储方式，即仅存储非零系数，可以减少内存消耗并提高性能。

        // 在Eigen中，SparseMatrix<>模板类是用于表示稀疏矩阵，注意稠密的用的是Matrix<>
        //SparseMatrix<>提供了高性能和低内存使用率。
        //它实现了广泛使用的压缩列（或行）存储方案的更通用的变体。

        //它由四个紧凑数组组成：
        // 1. Values: 存储非零的系数值。注意，压缩列存储按照一列一列的顺序存储非零元素，同样，压缩行按照一行一行的顺序存储元素；
        // 2. InnerIndices: 存储非零的行（或列）下标。简单的说，我们可以先按照列或者行的顺序存储values，然后对于values中的每一个元素，确定其所在的行or列的下标；
        // 3. OuterStarts: 为每列（分别为行）存储前两个数组中第一个非零的索引。注意，这个数组存储的Values和InnerIndices中的位置；
        // 4. InnerNNZs:存储每列（分别为行）的非零元素个数。
        // 注意：术语inner指的是内部向量，该向量是列主矩阵的列，或行主矩阵的行。
        // 术语outer是指另一个方向，即行or列；

        // 在一个示例中可以更好地解释此存储方案。以下矩阵
        // 0	3	0	0	0
        // 22	0	0	0	17
        // 7	5	0	1	0
        // 0	0	0	0	0
        // 0	0	14	0	8
        // 及其可能的稀疏**列主**表示形式之一：

        // Values:	22	7	_	3	5	14	_	_	1	_	17	8                注释：按列主方式，一列一列的存储非零元素， _ 是为了插入元素用的

        // InnerIndices:	1	2	_	0	2	4	_	_	2	_	1	4      注释：有了Values后，我们可以看Values元素所在的每一行，例如，22在第1行，7在第2行，以此类推


        // OuterStarts:	0	3	5	8	10	12      注释：OutStarts表示的每一列中的起始非零元素在Values or InnerIndices中的索引，
        // 例如,第一列非零元素22在Values中的第0索引，第二列中的3在Values中的第3索引，第三列中的14在Values中的第5索引...
        // 令我困惑的是，OutStarts多了一个元素，比如上述方阵有5列，那么应该只有5个元素，但是它有6个元素，难道是存储了最后一个元素？


        // InnerNNZs:	2	2	1	1	2       注释：5个列中的非零元素个数


        // TODO:
        // 当前，确保始终通过增加内部索引来对给定内部向量的元素进行排序。
        //该"_"指示可用空间来快速插入新的元素。假设不需要重新分配，
        //则在O（nnz_j）中插入随机元素，其中nnz_j是相应内部向量的非零数。
        //另一方面，在给定的内部向量中插入具有增加的内部索引的元素会更加有效，
        //因为这仅需要增加InnerNNZs作为O（1）运算的各个元素。

        // 没有可用空间的情况是一种特殊情况，称为压缩模式。
        //它对应于广泛使用的压缩列（或行）存储方案（CCS或CRS）。
        //通过调用SparseMatrix :: makeCompressed（）函数，可以将任何SparseMatrix转换为这种形式。
        //在这种情况下，因为我们等于：[j] = [j + 1]- [j] ，所以可以指出该数组是多余的。
        //因此，实际上，对SparseMatrix :: makeCompressed（）的调用将释放此缓冲区。
        //InnerNNZsOuterStartsInnerNNZsOuterStartsOuterStarts
        // 值得注意的是，我们对外部库的大多数包装都需要压缩矩阵作为输入。

        //Eigen运算的结果始终会生成压缩的稀疏矩阵。
        //另一方面，在SparseMatrix中插入新元素会将其稍后转换为非压缩模式。
        // 这是以前以压缩模式表示的矩阵：

        // Values:	22	7	3	5	14	1	17	8
        // InnerIndices:	1	2	0	2	4	2	1	4
        // OuterStarts:	        0	2	4	5	6	8
        // 一个稀疏向量是一个的一种特殊情况稀疏矩阵，其中只有Values和InnerIndices数组存储。
        //SparseVector没有压缩/未压缩模式的概念。

        // 第一个例子
        // 在描述每个类之前，让我们从以下典型示例开始：
        //$ \ Delta u = 0 $使用有限差分方案和Dirichlet边界条件在规则2D网格上求解拉普拉斯方程。
        //这样的问题可以在数学上表示为Ax = b形式的线性问题，其中x是一个m维向量，在本例中，它表示像素值, b是从边界条件中获得，
        //并且A是一个mXm的方阵，它是从离散化的拉普拉斯算子中获得的稀疏矩阵，即，只含有少数的非0元素

        void FirstExample() {
            LOG();

            int n = 300;   // size of the image
            int m = n * n; // number of unknows (=number of pixels)
            // Assembly:
            std::vector<T> coefficients;      // list of non-zeros coefficients,在HelpFunctions.hpp中的```typedef Eigen::Triplet<double> T;```
            Eigen::VectorXd b(m);             // the right hand side-vector resulting from the constraints
            buildProblem(coefficients, b,
                         n); //在HelpFunctions.hpp中的```void buildProblem(std::vector<T> &coefficients, Eigen::VectorXd &b, int n);```
            SpMat A(m, m);
            A.setFromTriplets(coefficients.begin(), coefficients.end());
            // Solving:
            Eigen::SimplicialCholesky<SpMat> chol(A); // performs a Cholesky factorization of A,即首先执行稀疏乔里斯基分解
            Eigen::VectorXd x = chol.solve(
                    b);        // use the factorization to solve for the given right hand side，然后求解Ax=b

            // Export the result to a file:
            saveAsBitmap(x, n, "result.bmp");
        }

        void TheSparseMatrixClass() {
            LOG();

            //  稀疏矩阵和矢量是一模板类，具有三种模板参数：
            //   1. Scaler（例如，double）,
            //   2. 存储顺序（例如，ColMajor或RowMajor，默认是ColMajor)
            //   3. 内索引类型（默认为int）

            // 类似于Matrix对象，构造函数参数为对象的大小，例如：
            SparseMatrix<std::complex<float>> mat_c(1000,
                                                    2000); // declares a 1000x2000 column-major compressed sparse matrix of complex<float>
            SparseMatrix<double, RowMajor> mat(1000,
                                               2000);      // declares a 1000x2000 row-major compressed sparse matrix of double
            SparseVector<std::complex<float>> vec_c(
                    1000);       // declares a column sparse vector of complex<float> of size 1000
            SparseVector<double, RowMajor> vec(1000);            // declares a row sparse vector of double of size 1000

            // 维数可以使用下面的方法获得：
            // Standard    dimensions，标准的维度
            // mat.rows()
            // mat.cols()
            // vec.size()

            // Sizes along the  inner/outer dimensions，内部和外部维度的大小
            // mat.innerSize()
            // mat.outerSize()

            // Number of non zero coefficients，非零系数的个数
            // mat.nonZeros()
            // vec.nonZeros()

            //  迭代非零系数:
            //随机访问的稀疏对象的元素可以通过来完成coeffRef(i,j)功能。
            //但是，此功能涉及相当昂贵的二进制搜索。在大多数情况下，一个人只想遍历非零元素。
            //这是通过在外部尺寸上进行标准循环，然后通过InnerIterator对当前内部向量的非零值进行迭代来实现的。
            //因此，必须以与存储顺序相同的顺序访问非零条目。这是一个例子：

            {
                const int rows = 5, cols = 5;
                SparseMatrix<double> mat(rows, cols);
                for (int k = 0; k < mat.outerSize(); ++k) {
                    for (SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
                        it.value();
                        it.row();   // row index
                        it.col();   // col index (here it is equal to k)
                        it.index(); // inner index, here it is equal to it.row()
                    }
                }
            }
            //对于可写表达式，可以使用valueRef（）函数修改参考值。如果稀疏矩阵或向量的类型取决于模板参数，则typename需要关键字来指示InnerIterator表示类型；
        }

        void FillingASparseMatrix() {
            //由于SparseMatrix的特殊存储方案，因此在添加新的非零条目时必须格外小心。例如，将单个纯随机插入SparseMatrix的成本为O(nnz)，其中nnz是非零系数的当前数量。

            // 因此，在确保良好性能的同时创建稀疏矩阵的最简单方法是:
            // 首先构建所谓的三元组列表，然后将其转换为SparseMatrix。
            // 这是一个典型的用法示例：

            // typedef Eigen::Triplet<double> T;
            // std::vector<T> tripletList;
            // tripletList.reserve(estimation_of_entries);
            // for (...)
            // {
            //         // ...
            //         tripletList.push_back(T(i, j, v_ij));
            // }
            // SparseMatrixType mat(rows, cols);
            // mat.setFromTriplets(tripletList.begin(), tripletList.end());
            // mat is ready to go!
        }

    } // namespace Section1_SparseMatrixManipulations
    //在Eigen中，当系数矩阵稀疏时，有几种方法可用于求解线性系统。
    // 由于此类矩阵的特殊表示，因此应格外小心，以获得良好的性能。
    // 有关Eigen中稀疏矩阵的详细介绍，请参见稀疏矩阵操作。
    //此页面列出了Eigen中可用的稀疏求解器。还介绍了所有这些线性求解器共有的主要步骤。
    //取决于矩阵的属性，所需的精度，最终用户能够调整这些步骤，以提高其代码的性能。
    //请注意，并不需要深入了解这些步骤背后的内容：
    //最后一部分提供了一个基准例程，可以轻松地使用它来深入了解所有可用求解器的性能。
    namespace Section2_SolveSparseLinearSystems {

        void ListofSparseSolvers() {
            //  Here SPD means symmetric positive definite SPD对应对称正定矩阵
            //Eigen当前提供了广泛的内置求解器，以及外部求解器库的包装器。下表中汇总了它们：

            // 1. 内置直接求解器: LLT LDLT需要对阵正定矩阵，LU需要方阵，QR适用于任意矩阵

            // Class	Solver kind	Matrix kind	Features related to performance	License        Notes

            // SimplicialLLT
            // #include<Eigen/SparseCholesky>	Direct LLt factorization	SPD	Fill-in reducing	LGPL    SimplicialLDLT is often preferable

            // SimplicialLDLT
            // #include<Eigen/SparseCholesky>	Direct LDLt factorization	SPD	Fill-in reducing	LGPL  Recommended for very sparse and not too large problems (e.g., 2D Poisson eq.)

            // SparseLU
            // #include<Eigen/SparseLU>	LU factorization	Square	Fill-in reducing, Leverage fast dense algebra	MPL2   optimized for small and large problems with irregular patterns

            // SparseQR
            // #include<Eigen/SparseQR>	QR factorization	Any, rectangular	Fill-in reducing	MPL2	recommended for least-square problems, has a basic rank-revealing feature

            // 2.内置迭代求解器

            //  Class	Solver kind	Matrix kind	Supported preconditioners, [default]	License  Notes

            // ConjugateGradient
            // #include<Eigen/IterativeLinearSolvers>	Classic iterative CG	SPD	IdentityPreconditioner, [DiagonalPreconditioner], IncompleteCholesky	MPL2
            // Recommended for large symmetric problems (e.g., 3D Poisson eq.)

            // LeastSquaresConjugateGradient
            // #include<Eigen/IterativeLinearSolvers>	CG for rectangular least-square problem	Rectangular	IdentityPreconditioner, [LeastSquareDiagonalPreconditioner]	MPL2
            // Solve for min |A'Ax-b|^2 without forming A'A

            // BiCGSTAB
            // #include<Eigen/IterativeLinearSolvers>	Iterative stabilized bi-conjugate gradient	Square	IdentityPreconditioner, [DiagonalPreconditioner], IncompleteLUT	MPL2	To speedup the convergence, try it with the IncompleteLUT preconditioner.

            //3.外部求解器
            //         Class	Module	Solver kind	Matrix kind	Features related to performance	Dependencies,License   Notes
            // PastixLLT
            // PastixLDLT
            // PastixLU	PaStiXSupport	Direct LLt, LDLt, LU factorizations	SPD
            // SPD
            // Square	Fill-in reducing, Leverage fast dense algebra, Multithreading	Requires the PaStiX package, CeCILL-C	optimized for tough problems and symmetric patterns
            // CholmodSupernodalLLT	CholmodSupport	Direct LLt factorization	SPD	Fill-in reducing, Leverage fast dense algebra	Requires the SuiteSparse package, GPL
            // UmfPackLU	UmfPackSupport	Direct LU factorization	Square	Fill-in reducing, Leverage fast dense algebra	Requires the SuiteSparse package, GPL
            // SuperLU	SuperLUSupport	Direct LU factorization	Square	Fill-in reducing, Leverage fast dense algebra	Requires the SuperLU library, (BSD-like)
            // SPQR	SPQRSupport	QR factorization	Any, rectangular	fill-in reducing, multithreaded, fast dense algebra	requires the SuiteSparse package, GPL	recommended for linear least-squares problems, has a rank-revealing feature
            // PardisoLLT
            // PardisoLDLT
            // PardisoLU	PardisoSupport	Direct LLt, LDLt, LU factorizations	SPD
            // SPD
            // Square	Fill-in reducing, Leverage fast dense algebra, Multithreading	Requires the Intel MKL package, Proprietary	optimized for tough problems patterns, see also using MKL with Eigen
        }

        void SparseSolverConcept() {
            // 一个通用的例子
            // #include <Eigen/RequiredModuleName>
            // // ...
            // SparseMatrix<double> A;
            // // fill A
            // VectorXd b, x;
            // // fill b
            // // solve Ax = b
            // SolverClassName<SparseMatrix<double>> solver;
            // solver.compute(A);
            // if (solver.info() != Success)
            // {
            //         // decomposition failed
            //         return;
            // }
            // x = solver.solve(b);
            // if (solver.info() != Success)
            // {
            //         // solving failed
            //         return;
            // }
            // // solve for another right hand side:
            // x1 = solver.solve(b1);

            // 对于SPD求解器，第二个可选模板参数允许指定必须使用哪个三角形部分，例如：
            //```
            // #include <Eigen / IterativeLinearSolvers>
            // ConjugateGradient <SparseMatrix <double>，Eigen :: Upper >求解器;
            // x = Solver.compute（A）.solve（b）;

            // ```
            // 在上面的示例中，仅考虑输入矩阵A的上三角部分进行求解。对面的三角形可能为空或包含任意值。

            // 在必须解决具有相同稀疏模式的多个问题的情况下，可以将“计算”步骤分解如下：

            // SolverClassName<SparseMatrix<double>> 求解器;
            // Solver.analyzePattern（A）; //对于此步骤，不使用A的数值
            // resolver.factorize（A）;
            // x1 = Solver.solve（b1）;
            // x2 = Solver.solve（b2）;
            // ... A = ...; //修改A的非零值，非零模式必须保持不变
            // resolver.factorize（A）;
            // x1 = Solver.solve（b1）;
            // x2 = Solver.solve（b2）;
            // ...

            // 该compute（）方法等效于调用analyzerPattern（）和factorize（）。

            // 每个求解器都提供一些特定功能，例如行列式，对因子的访问，迭代控制等。有关更多详细信息，请参见相应类的文档。

            // 最后，大多数迭代求解器也可以在无矩阵的上下文中使用，请参见以下示例。
        }

        void TheComputeStep() {
            // 在compute（）函数中，通常对矩阵进行因子分解：LLT用于自伴随矩阵，LDLT用于普通埃尔米特矩阵，LU用于非埃尔米特矩阵，QR用于矩形矩阵。
            //  这些是使用直接求解器的结果。对于这类求解器，将计算步骤进一步细分为analyzerPattern（）和factorize（）。

            // analyticsPattern（）的目标是对矩阵的非零元素进行重新排序，以使分解步骤创建的填充更少。
            //此步骤仅利用矩阵的结构。因此，该步骤的结果可用于矩阵结构相同的其他线性系统。
            //但是请注意，有时某些外部求解器（例如SuperLU）要求在此步骤中设置矩阵的值，以平衡矩阵的行和列。
            // 在这种情况下，此步骤的结果不应与其他矩阵一起使用。

            // Eigen在此步骤中提供了一组有限的方法来对矩阵进行重新排序，这些方法可以是内置的（COLAMD，AMD）或外部的（METIS）。
            // 这些方法在求解器的模板参数列表中设置：

            // DirectSolverClassName <SparseMatrix <double>，OrderingMethod <IndexType>> solver;
            // 有关可用方法和相关选项的列表，请参见OrderingMethods模块。

            // 在factorize（）中，计算系数矩阵的因数。每当矩阵值更改时，都应调用此步骤。但是，矩阵的结构模式不应在多个调用之间改变。

            // 对于迭代求解器，计算步骤用于最终设置预处理器。例如，使用ILUT预处理器，在此步骤中计算不完全因子L和U。
            // 请记住，基本上，预处理器的目标是通过解决系数矩阵具有更多聚类特征值的改进线性系统来加快迭代方法的收敛速度。
            //对于实际问题，迭代求解器应始终与前置条件一起使用。在Eigen中，只需将预处理器作为模板参数添加到迭代求解器对象中，即可选择预处理器。

            // IterativeSolverClassName <SparseMatrix <double>，PreconditionerName <SparseMatrix <double>>求解器;
            // 成员函数preconditioner（）返回对preconditioner的读写引用以直接与其交互。有关可用方法的列表，请参见迭代求解器模块和每个类的文档。
        }

        void TheSolveStep() {
            //resolve()函数计算具有一个或多个右侧的线性系统的解。
            // X = Solver.solve（B）;
            // 在此，B可以是向量或矩阵，其中各列形成不同的右侧。可以多次调用solve（）函数，例如，当所有右侧都无法同时使用时。

            // x1 = Solver.solve（b1）;
            // //获得第二个右侧b2
            // x2 = Solver.solve（b2）;
            // // ...
            // 对于直接方法，解决方案以机器精度计算。
            // 有时，解决方案不必太精确。在这种情况下，迭代方法更合适，并且可以在使用setTolerance（）的求解步骤之前设置所需的精度。
            // 有关所有可用功能，请参阅“ 迭代求解器”模块的文档。
        }

        void BenchmarkRoutine() {

            // 大多数时候，您需要知道的是解决系统所需的时间，并希望什么是最合适的求解器。
            // 在Eigen中，我们提供了可用于此目的的基准例程。这是非常容易使用。
            // 在构建目录中，导航到bench / spbench并通过键入make spbenchsolver来编译例程。
            // 使用–help选项运行它以获取所有可用选项的列表。基本上，要测试的矩阵应为MatrixMarket坐标格式，并且例程从Eigen中所有可用求解器返回统计信息。
            // 要以矩阵市场格式导出矩阵和右侧矢量，可以使用不受支持的SparseExtra模块：
            // #include <unsupported / Eigen / SparseExtra>
            // ...
            // Eigen :: saveMarket（A，“ filename.mtx”）;
            // Eigen :: saveMarket（A，“ filename_SPD.mtx”，Eigen :: Symmetric）; //如果A为对称正定
            // Eigen :: saveMarketVector（B，“ filename_b.mtx”）;
            // 下表提供了一些Eigen内置和外部求解器的XML统计信息示例。
            // 参见文档，这里不列出了
        }
    } // namespace Section2_SolveSparseLinearSystems
    namespace Section3_MatrixFreeSolvers {

        // 诸如ConjugateGradient和BiCGSTAB之类的迭代求解器可以在无矩阵上下文中使用。
        // 为此，用户必须提供一个继承EigenBase <>并实现以下方法的包装器类：

        // Index rows()和Index cols()：分别返回行数和列数
        // operator* 带有您的类型和Eigen密集列向量（其实际实现在internal :: generic_product_impl类的特殊化中进行）
        // Eigen::internal::traits<> 还必须专门针对包装器类型。

        // 这是包装Eigen :: SparseMatrix的完整示例：

        // #include <iostream>
        // #include <Eigen/Core>
        // #include <Eigen/Dense>
        // #include <Eigen/IterativeLinearSolvers>
        // #include <unsupported/Eigen/IterativeSolvers>
        // class MatrixReplacement;
        // using Eigen::SparseMatrix;
        // namespace Eigen {
        // namespace internal {
        //   // MatrixReplacement looks-like a SparseMatrix, so let's inherits its traits:
        //   template<>
        //   struct traits<MatrixReplacement> :  public Eigen::internal::traits<Eigen::SparseMatrix<double> >
        //   {};
        // }
        // }
        // // Example of a matrix-free wrapper from a user type to Eigen's compatible type
        // // For the sake of simplicity, this example simply wrap a Eigen::SparseMatrix.
        // class MatrixReplacement : public Eigen::EigenBase<MatrixReplacement> {
        // public:
        //   // Required typedefs, constants, and method:
        //   typedef double Scalar;
        //   typedef double RealScalar;
        //   typedef int StorageIndex;
        //   enum {
        //     ColsAtCompileTime = Eigen::Dynamic,
        //     MaxColsAtCompileTime = Eigen::Dynamic,
        //     IsRowMajor = false
        //   };
        //   Index rows() const { return mp_mat->rows(); }
        //   Index cols() const { return mp_mat->cols(); }
        //   template<typename Rhs>
        //   Eigen::Product<MatrixReplacement,Rhs,Eigen::AliasFreeProduct> operator*(const Eigen::MatrixBase<Rhs>& x) const {
        //     return Eigen::Product<MatrixReplacement,Rhs,Eigen::AliasFreeProduct>(*this, x.derived());
        //   }
        //   // Custom API:
        //   MatrixReplacement() : mp_mat(0) {}
        //   void attachMyMatrix(const SparseMatrix<double> &mat) {
        //     mp_mat = &mat;
        //   }
        //   const SparseMatrix<double> my_matrix() const { return *mp_mat; }
        // private:
        //   const SparseMatrix<double> *mp_mat;
        // };
        // // Implementation of MatrixReplacement * Eigen::DenseVector though a specialization of internal::generic_product_impl:
        // namespace Eigen {
        // namespace internal {
        //   template<typename Rhs>
        //   struct generic_product_impl<MatrixReplacement, Rhs, SparseShape, DenseShape, GemvProduct> // GEMV stands for matrix-vector
        //   : generic_product_impl_base<MatrixReplacement,Rhs,generic_product_impl<MatrixReplacement,Rhs> >
        //   {
        //     typedef typename Product<MatrixReplacement,Rhs>::Scalar Scalar;
        //     template<typename Dest>
        //     static void scaleAndAddTo(Dest& dst, const MatrixReplacement& lhs, const Rhs& rhs, const Scalar& alpha)
        //     {
        //       // This method should implement "dst += alpha * lhs * rhs" inplace,
        //       // however, for iterative solvers, alpha is always equal to 1, so let's not bother about it.
        //       assert(alpha==Scalar(1) && "scaling is not implemented");
        //       EIGEN_ONLY_USED_FOR_DEBUG(alpha);
        //       // Here we could simply call dst.noalias() += lhs.my_matrix() * rhs,
        //       // but let's do something fancier (and less efficient):
        //       for(Index i=0; i<lhs.cols(); ++i)
        //         dst += rhs(i) * lhs.my_matrix().col(i);
        //     }
        //   };
        // }
        // }
        // int main()
        // {
        //   int n = 10;
        //   Eigen::SparseMatrix<double> S = Eigen::MatrixXd::Random(n,n).sparseView(0.5,1);
        //   S = S.transpose()*S;
        //   MatrixReplacement A;
        //   A.attachMyMatrix(S);
        //   Eigen::VectorXd b(n), x;
        //   b.setRandom();
        //   // Solve Ax = b using various iterative solver with matrix-free version:
        //   {
        //     Eigen::ConjugateGradient<MatrixReplacement, Eigen::Lower|Eigen::Upper, Eigen::IdentityPreconditioner> cg;
        //     cg.compute(A);
        //     x = cg.solve(b);
        //     std::cout << "CG:       #iterations: " << cg.iterations() << ", estimated error: " << cg.error() << std::endl;
        //   }
        //   {
        //     Eigen::BiCGSTAB<MatrixReplacement, Eigen::IdentityPreconditioner> bicg;
        //     bicg.compute(A);
        //     x = bicg.solve(b);
        //     std::cout << "BiCGSTAB: #iterations: " << bicg.iterations() << ", estimated error: " << bicg.error() << std::endl;
        //   }
        //   {
        //     Eigen::GMRES<MatrixReplacement, Eigen::IdentityPreconditioner> gmres;
        //     gmres.compute(A);
        //     x = gmres.solve(b);
        //     std::cout << "GMRES:    #iterations: " << gmres.iterations() << ", estimated error: " << gmres.error() << std::endl;
        //   }
        //   {
        //     Eigen::DGMRES<MatrixReplacement, Eigen::IdentityPreconditioner> gmres;
        //     gmres.compute(A);
        //     x = gmres.solve(b);
        //     std::cout << "DGMRES:   #iterations: " << gmres.iterations() << ", estimated error: " << gmres.error() << std::endl;
        //   }
        //   {
        //     Eigen::MINRES<MatrixReplacement, Eigen::Lower|Eigen::Upper, Eigen::IdentityPreconditioner> minres;
        //     minres.compute(A);
        //     x = minres.solve(b);
        //     std::cout << "MINRES:   #iterations: " << minres.iterations() << ", estimated error: " << minres.error() << std::endl;
        //   }
        // }

        // 输出：

        // CG:       #iterations: 20, estimated error: 8.86333e-14
        // BiCGSTAB: #iterations: 20, estimated error: 2.10809e-15
        // GMRES:    #iterations: 10, estimated error: 0
        // DGMRES:   #iterations: 20, estimated error: 1.10455e-28
        // MINRES:   #iterations: 20, estimated error: 2.94473e-14
    } // namespace Section3_MatrixFreeSolvers
} // namespace Chapter3_SparseLinearAlgebra
void TestChapter3Section1() {
    Chapter3_SparseLinearAlgebra::Section1_SparseMatrixManipulations::FirstExample();
}

void TestChapter3Section2() {

    Chapter3_SparseLinearAlgebra::Section1_SparseMatrixManipulations::TheSparseMatrixClass();
}

int main() {
    //build pass
    TestChapter3Section1();   // 会生成一张图片，titiled  "result.bmp"

    TestChapter3Section2();
    return 0;
}