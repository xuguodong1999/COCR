/**
 * https://github.com/qixianyu-buaa/EigenChineseDocument/tree/master/Eigen/Chapter2_DenseLinearProblemsAndDecompositions
 */
#include <iostream>
// The Eigen/Dense header file defines all member functions for the MatrixXd type and related types
#include <Eigen/Dense>
//All classes and functions defined in this header file (and other Eigen header files) are in the Eigen namespace.

#define LOG()  printf("\n=========  %s() Output is  ========= \n",__FUNCTION__);

using namespace Eigen;
using namespace std;
namespace Chapter2_DenseLinearProblemsAndDecompositions {

    namespace Section1_LinearAlgebraAndDecompositions {
        //本页说明了如何求解线性系统，计算各种分解，例如LU，QR，SVD，本征分解...阅读此页后，请不要错过我们的密集矩阵分解目录。
        //基本线性求解 Ax=b
        //该解决方案：可将各种分解之间进行选择，取决于你的矩阵一个样子，取决于你是否赞成速度或准确性。
        //但是，让我们从一个适用于所有情况的示例开始，这是一个很好的折衷方案：
        void BasicLinearSolving() {
            LOG();
            Matrix3f A;
            Vector3f b;
            A << 1, 2, 3, 4, 5, 6, 7, 8, 10;
            b << 3, 3, 4;
            cout << "Here is the matrix A:\n"
                 << A << endl;
            cout << "Here is the vector b:\n"
                 << b << endl;
            Vector3f x = A.colPivHouseholderQr().solve(b);
            cout << "The solution is:\n"
                 << x << endl;

            // Output is:
            // Here is the matrix A:
            //  1  2  3
            //  4  5  6
            //  7  8 10
            // Here is the vector b:
            // 3
            // 3
            // 4
            // The solution is:
            // -2
            //  1
            //  1

            //在此示例中，colPivHouseholderQr（）方法返回类ColPivHouseholderQR的对象。
            //由于此处的矩阵类型为Matrix3f，因此该行可能已替换为：
            //ColPivHouseholderQR <Matrix3f> dec（A）;
            //Vector3f x = dec.solve（b）;
            //在这里，ColPivHouseholderQR是具有选择列主元功能的QR分解。
            //这是本教程的一个不错的折衷方案，因为它适用于所有矩阵，而且速度非常快。
            //这是一些其他分解表，您可以根据矩阵和要进行的权衡选择：

            // Decomposition	Method	                Requirements on the matrix	                        Speed(small-to-medium)	                Speed (large)	                Accuracy
            // PartialPivLU	         partialPivLu()	                    Invertible	                                                                        ++	                                                ++	                                        +
            // FullPivLU	                fullPivLu()	                        None	                                                                            -	                                                     - -	                                   +++
            // HouseholderQR	householderQr()	              None	                                                                        ++	                                                ++                                              +
            // ColPivHouseholderQR	colPivHouseholderQr()	None	                                                                +	                                                     -	                                           +++
            // FullPivHouseholderQR	fullPivHouseholderQr()	None                                                            	-	                                                    - -	                                                +++
            // CompleteOrthogonalDecomposition	completeOrthogonalDecomposition()	None	+	-	+++
            // LLT	llt()	                                                                Positive definite	                                                                +++	+++	+
            // LDLT	ldlt()	                                        Positive or negative semidefinite	                                                +++	+	++
            // BDCSVD	bdcSvd()	None	-	-	+++
            // JacobiSVD	jacobiSvd()	None	-	- - -	+++

            {
                //所有这些分解都提供了一个solve（）方法，该方法与上述示例一样。
                //例如，如果您的矩阵是正定的，则​​上表说明LLT或LDLT分解是一个很好的选择。这是一个示例，也说明使用通用矩阵（而非矢量）作为右手边是可能的。
                Matrix2f A, b;
                A << 2, -1, -1, 3;
                b << 1, 2, 3, 1;
                cout << "Here is the matrix A:\n"
                     << A << endl;
                cout << "Here is the right hand side b:\n"
                     << b << endl;
                Matrix2f x = A.ldlt().solve(b);
                cout << "The solution is:\n"
                     << x << endl;
                //Output is:
                // Here is the matrix A:
                //  2 -1
                // -1  3
                // Here is the right hand side b:
                // 1 2
                // 3 1
                // The solution is:
                // 1.2 1.4
                // 1.4 0.8

                // Eigen 矩阵分解：http://eigen.tuxfamily.org/dox/group__TopicLinearAlgebraDecompositions.html
            }
        }

        void CheckingIfASolutionReallyExists() {
            LOG();
            //计算相对误差的方法
            //只有您知道要允许解决方案被视为有效的误差范围。因此，Eigen允许您自己进行此计算，如以下示例所示：
            MatrixXd A = MatrixXd::Random(100, 100);
            MatrixXd b = MatrixXd::Random(100, 50);
            MatrixXd x = A.fullPivLu().solve(b);
            double relative_error = (A * x - b).norm() / b.norm(); // norm() is L2 norm
            cout << "The relative error is:\n"
                 << relative_error << endl;
            // Output is:
            // The relative error is:
            // 2.31495e-14
        }

        void ComputingEigenvaluesAndEigenvectors() {
            LOG();
            //您需要在此处进行特征分解， 确保检查矩阵是否是自伴随的，
            //在数学里，作用于一个有限维的内积空间，一个自伴算子(self-adjoint operator)等于自己的伴随算子；等价地说，表达自伴算子的矩阵是埃尔米特矩阵。埃尔米特矩阵等于自己的共轭转置。根据有限维的谱定理，必定存在着一个正交归一基，可以表达自伴算子为一个实值的对角矩阵。
            // 就像在这些问题中经常发生的那样。这是一个使用SelfAdjointEigenSolver的示例，可以使用EigenSolver或ComplexEigenSolver轻松地将其应用于一般矩阵。
            //特征值和特征向量的计算不一定会收敛，但是这种收敛失败的情况很少。调用info（）就是为了检查这种可能性。
            Matrix2f A;
            A << 1, 2, 2, 3;
            cout << "Here is the matrix A:\n"
                 << A << endl;
            SelfAdjointEigenSolver<Matrix2f> eigensolver(A);
            if (eigensolver.info() != Success)
                abort();
            cout << "The eigenvalues of A are:\n"
                 << eigensolver.eigenvalues() << endl;
            cout << "Here's a matrix whose columns are eigenvectors of A \n"
                 << "corresponding to these eigenvalues:\n"
                 << eigensolver.eigenvectors() << endl;
            //Output is:
            // Here is the matrix A:
            // 1 2
            // 2 3
            // The eigenvalues of A are:
            // -0.236
            //   4.24
            // Here's a matrix whose columns are eigenvectors of A
            // corresponding to these eigenvalues:
            // -0.851 -0.526
            //  0.526 -0.851
        }

        void ComputingInverseAndDeterminant() {
            LOG();
            //首先，请确保您确实想要这个。尽管逆和行列式是基本的数学概念，但在数值线性代数中，它们不如在纯数学中流行。
            //逆计算通常可以用solve（）操作代替，而行列式通常不是检查矩阵是否可逆的好方法。
            //但是，对于非常 小的矩阵，上述条件是不正确的，并且逆和行列式可能非常有用。
            //尽管某些分解（例如PartialPivLU和FullPivLU）提供了inverse（）和determinant（）方法，
            //但您也可以直接在矩阵上调用inverse（）和determinant（）。
            //如果矩阵的固定大小很小（最多为4x4），这将使Eigen避免执行LU分解，而应使用对此类小矩阵更有效的公式。
            Matrix3f A;
            A << 1, 2, 1,
                    2, 1, 0,
                    -1, 1, 2;
            cout << "Here is the matrix A:\n"
                 << A << endl;
            cout << "The determinant of A is " << A.determinant() << endl;
            cout << "The inverse of A is:\n"
                 << A.inverse() << endl;

            // Output is:
            // Here is the matrix A:
            //  1  2  1
            //  2  1  0
            // -1  1  2
            // The determinant of A is -3
            // The inverse of A is:
            // -0.667      1  0.333
            //   1.33     -1 -0.667
            //     -1      1      1
        }

        void LeastSquaresSolving() {
            LOG();
            //最小二乘求解的最准确方法是SVD分解。Eigen提供了两种实现。
            //推荐的对象是BDCSVD类，它可以很好地解决较大的问题，并自动退回到JacobiSVD类以解决较小的问题。
            //对于这两个类，它们的resolve（）方法都在进行最小二乘求解。

            MatrixXf A = MatrixXf::Random(3, 2);
            cout << "Here is the matrix A:\n"
                 << A << endl;
            VectorXf b = VectorXf::Random(3);
            cout << "Here is the right hand side b:\n"
                 << b << endl;
            cout << "The least-squares solution is:\n"
                 << A.bdcSvd(ComputeThinU | ComputeThinV).solve(b) << endl;

            // Output is:
            // Here is the matrix A:
            //   0.68  0.597
            // -0.211  0.823
            //  0.566 -0.605
            // Here is the right hand side b:
            //  -0.33
            //  0.536
            // -0.444
            // The least-squares solution is:
            // -0.67
            // 0.314
            //可能更快但可靠性较低的另一种方法是使用矩阵的Cholesky分解或QR分解。我们关于最小二乘法求解的页面有更多详细信息。
        }

        void SeparatingTheComputationFromTheConstruction() {
            LOG();
            // 在以上示例中，在构造分解对象的同时计算了分解。但是，在某些情况下，您可能希望将这两件事分开，
            //例如，如果在构造时不知道要分解的矩阵，则可能会需要将它们分开。或者您想重用现有的分解对象。
            // 使之成为可能的原因是：
            // 所有分解都有默认的构造函数，
            // 所有分解都具有执行计算的compute（matrix）方法，并且可以在已计算的分解中再次调用该方法，以将其重新初始化。
            Matrix2f A, b;
            LLT<Matrix2f> llt;
            A << 2, -1, -1, 3;
            b << 1, 2, 3, 1;
            cout << "Here is the matrix A:\n"
                 << A << endl;
            cout << "Here is the right hand side b:\n"
                 << b << endl;
            cout << "Computing LLT decomposition..." << endl;
            llt.compute(A);
            cout << "The solution is:\n"
                 << llt.solve(b) << endl;
            A(1, 1)
                    ++;
            cout << "The matrix A is now:\n"
                 << A << endl;
            cout << "Computing LLT decomposition..." << endl;
            llt.compute(A);
            cout << "The solution is now:\n"
                 << llt.solve(b) << endl;
            //最后，您可以告诉分解构造函数预先分配存储空间以分解给定大小的矩阵，以便在随后分解此类矩阵时，不执行动态内存分配（当然，如果您使用的是固定大小的矩阵，则不存在动态内存分配完全发生）。
            //只需将大小传递给分解构造函数即可完成，如以下示例所示：
            {
                HouseholderQR<MatrixXf> qr(50, 50);
                MatrixXf A = MatrixXf::Random(50, 50);
                qr.compute(A); //没有动态内存分配
            }
        }

        void RankRevealingDecompositions() {
            LOG();
            //某些分解是揭示矩阵秩的。
            //这些通常也是在非满秩矩阵（在方形情况下表示奇异矩阵）的情况下表现最佳的分解。
            //秩揭示分解至少提供了rank()方法。它们还可以提供方便的方法，例如isInvertible()，
            //并且还提供一些方法来计算矩阵的核（零空间）和像（列空间），就像FullPivLU那样：
            Matrix3f A;
            A << 1, 2, 5,
                    2, 1, 4,
                    3, 0, 3;
            cout << "Here is the matrix A:\n"
                 << A << endl;
            FullPivLU<Matrix3f> lu_decomp(A);
            cout << "The rank of A is " << lu_decomp.rank() << endl;
            cout << "Here is a matrix whose columns form a basis of the null-space of A:\n"
                 << lu_decomp.kernel() << endl;
            cout << "Here is a matrix whose columns form a basis of the column-space of A:\n"
                 << lu_decomp.image(A) << endl; // yes, have to pass the original A

            // Output is:
            // Here is the matrix A:
            // 1 2 5
            // 2 1 4
            // 3 0 3
            // The rank of A is 2
            // Here is a matrix whose columns form a basis of the null-space of A:
            //  0.5
            //    1
            // -0.5
            // Here is a matrix whose columns form a basis of the column-space of A:
            // 5 1
            // 4 2
            // 3 3

            //当然，任何秩计算都取决于对任意阈值的选择，因为实际上没有浮点矩阵恰好是秩不足的。
            //Eigen选择一个明智的默认阈值，该阈值取决于分解，但通常是对角线大小乘以机器ε。
            //虽然这是我们可以选择的最佳默认值，但只有您知道您的应用程序的正确阈值是多少。
            //您可以通过在调用rank（）或需要使用此阈值的任何其他方法之前在分解对象上调用setThreshold（）来进行设置。
            //分解本身（即compute（）方法）与阈值无关。更改阈值后，无需重新计算分解.
            {
                Matrix2d A;
                A << 2, 1,
                        2, 0.9999999999;
                FullPivLU<Matrix2d> lu(A);
                cout << "By default, the rank of A is found to be " << lu.rank() << endl;
                lu.setThreshold(1e-5);
                cout << "With threshold 1e-5, the rank of A is found to be " << lu.rank() << endl;

                // Output is:
                // By default, the rank of A is found to be 2
                // With threshold 1e-5, the rank of A is found to be 1
            }
        }

    } // namespace Section1_LinearAlgebraAndDecompositions
    namespace Section2_CatalogueOfDenseDecompositions {
        // 此页面显示了Eigen提供的稠密矩阵分解的目录。

        // 分解	矩阵要求  速度	 算法的可靠性和准确性	可计算矩阵秩	允许计算（除了线性求解） Eigen提供的线性求解器	Eigen实施的成熟度   最佳化

        // 这个分解类似于没有行交换的A=LU
        // PartialPivLU	可逆的	快速	取决于条件数	--	--	是	优秀的	  Blocking，隐式MT

        //这个分解类似于PA=LU
        // FullPivLU	--	慢	久经考验	是	--	是	优秀的	 --

        // 一般的矩阵QR分解要求特征向量线性无关，不知道为啥这个QR没要求
        // HouseholderQR	--	快速	取决于条件数	--	正交化	是	优秀的	Blocking

        //列主元QR分解？
        // ColPivHouseholderQR	--	快速	好	是	正交化	是	优秀的	很快：Blocking

        //全主元QR分解
        // FullPivHouseholderQR	--	慢	久经考验	是	正交化	是	平均	--

        // LLT	正定	非常快	取决于条件数	--	--	是	优秀的	 Blocking

        // LDLT	正或负半定	非常快	   好    	--	--	是	优秀的	 很快：Blocking

        // 奇异值和特征值分解

        // BDCSVD（分而治之）	--	最快的SVD算法之一	优秀的	是	奇异值/向量，最小二乘	是的（并且最小二乘）	优秀的	阻断  双对角化

        // JacobiSVD（双面）	--	慢（但对于小型矩阵则快）	证明3	是	奇异值/向量，最小二乘	是的（并且最小二乘）	优秀的	R-SVD

        // 自伴特征算法	自伴	快速平均2	好	是	特征值/向量	--	优秀的	2x2和3x3的解析解

        // 复杂特征求解器	平方	慢-非常慢2	取决于条件数	是	特征值/向量	--	一般	--

        // 特征解算器	正方形和实数	平均-慢  	取决于条件数	是	特征值/向量	--	一般	--

        // 广义自我伴随特征求解	  方阵	快速-一般2	取决于条件数	--	广义特征值/向量	--	好	 --

        // 辅助分解

        // RealSchur	正方形和实数	一般-慢2	取决于条件数	是	--	--	一般	 --

        // ComplexSchur	正方形	慢-非常慢2	取决于条件数	是	--	--	一般	 --

        // Tridiagonalization	自伴	快速	好	--	--	--	好	很快：封锁

        // HessenbergDecomposition	正方形	平均	好	--	--	--	好	很快：封锁

        // 笔记：
        // 1：LDLT算法有两种变体。Eigen的一个生成纯对角D矩阵，因此它不能处理不定的矩阵，这与Lapack的一个生成块对角D矩阵不同。
        // 2：特征值，SVD和Schur分解依赖于迭代算法。它们的收敛速度取决于特征值的分离程度。
        // 3：我们的JacobiSVD是双面的，可为平方矩阵提供经过验证的最佳精度。对于非平方矩阵，我们必须首先使用QR预调节器。默认选择ColPivHouseholderQR已经非常可靠，但是如果您想证明这一点，请改用FullPivHouseholderQR。
        // 术语：
        // 自伴
        // 对于实矩阵，自伴是对称的同义词。对于复杂的矩阵，自伴为同义词埃尔米特。更一般地，A当且仅当矩阵等于其伴随时A*，矩阵才是自伴随的$ A ^ * $。伴随也称为共轭转置。
        // 正/负定
        // $ $如果$ v ^ * A v> 0 $对于任何非零向量，则自伴矩阵是正定的$ v $。同样，如果$ v ^ * A v <0 $对于任何非零向量，则为负定$ v $
        // 正/负半定
        // $ $如果$ v ^ * A v \ ge 0 $对于任何非零向量，自伴矩阵是正半定的$ v $。同样，$ v ^ * A v \ le 0 $对于任何非零向量，它为负半定数$ v $

        // Blocking
        // 意味着该算法可以按块工作，从而保证了大型矩阵性能的良好缩放。
        // 隐式多线程（MT）
        // 意味着该算法可以通过OpenMP利用多核处理器。“隐式”是指算法本身不是并行的，而是依赖于并行矩阵矩阵乘积的规则。
        // 显式多线程（MT）
        // 意味着该算法已显式并行化，以通过OpenMP利用多核处理器。
        // 元展开器
        // 意味着对于很小的固定大小矩阵，该算法将自动显式展开。
    } // namespace Section2_CatalogueOfDenseDecompositions
    namespace Section3_SolvingLinearLeastSquaresSystems {
        // 本页介绍如何使用本征求解线性最小二乘系统。一个超定方程组，例如Ax = b，没有解。
        //在这种情况下，在差异Ax - b尽可能小的意义上，搜索最接近解的向量x是有意义的。
        //该x称为最小二乘解（如果使用欧几里得范数）。
        //本页讨论的三种方法是SVD分解，QR分解和正态方程。
        //其中，SVD分解通常最准确但最慢，正则方程(normal equations)最快但最不准确，QR分解介于两者之间。

        void UsingTheSvdDecomposition() {
            LOG();
            //BDCSVD类中的solve()中方法可以直接用来求解线性最小二乘系统。
            //仅计算奇异值（此类的默认值）是不够的。您还需要奇异矢量，但是稀疏SVD分解足以计算最小二乘解：
            MatrixXf A = MatrixXf::Random(3, 2);
            cout << "Here is the matrix A:\n"
                 << A << endl;
            VectorXf b = VectorXf::Random(3);
            cout << "Here is the right hand side b:\n"
                 << b << endl;
            cout << "The least-squares solution is:\n"
                 << A.bdcSvd(ComputeThinU | ComputeThinV).solve(b) << endl;
            // Output is:
            // Here is the matrix A:
            //   0.68  0.597
            // -0.211  0.823
            //  0.566 -0.605
            // Here is the right hand side b:
            //  -0.33
            //  0.536
            // -0.444
            // The least-squares solution is:
            // -0.67
            // 0.314
        }

        void UsingTheQRDecomposition() {
            LOG();
            //QR分解类中的resolve()方法还计算最小二乘解。
            //有3种QR分解类：HouseholderQR（无pivoting，因此快速但不稳定），ColPivHouseholderQR（列枢轴，因此较慢但更准确）和FullPivHouseholderQR（全枢轴，因此最慢且最稳定）。
            MatrixXf A = MatrixXf::Random(3, 2);
            VectorXf b = VectorXf::Random(3);
            cout << "The solution using the QR decomposition is:\n"
                 << A.colPivHouseholderQr().solve(b) << endl;
            // Output is:
            // The solution using the QR decomposition is:
            // -0.67
            // 0.314
        }

        void UsingNormalEquations() {
            LOG();
            //找到Ax = b的最小二乘解等效于求解法线方程A T Ax = A T b。
            //如果矩阵A是病态的，那么这不是一个好方法，因为A T A的条件数是A的条件数的平方。
            //这意味着与使用其他方法相比，使用正则方程式丢失的数字要多两倍。
            MatrixXf A = MatrixXf::Random(3, 2);
            VectorXf b = VectorXf::Random(3);
            cout << "The solution using normal equations is:\n"
                 << (A.transpose() * A).ldlt().solve(A.transpose() * b) << endl;
        }
    } // namespace Section3_SolvingLinearLeastSquaresSystems
    //complete pivoting和partial pivoting
    // （1） complete pivoting（即full pivoting），就是在矩阵分解或高斯消元的过程中，主元是在未进行变换的所有行和列之间进行选择。也就是需要同时进行行交换和列交换。
    // （2） partial pivoting就是只在当前进行变换的列中选择主元，只需要进行行交换
    namespace Section4_InplaceDecompisitions {
        void InplaceMatrixDecompositions() {
            LOG();
            // 从Eigen 3.3开始，LU，Cholesky和QR分解可以就地进行操作，即直接在给定的输入矩阵内进行。
            //当处理大量矩阵或可用内存非常有限（嵌入式系统）时，此功能特别有用。

            // 为此，必须使用Ref <>矩阵类型实例化各个分解类，
            //并且必须使用输入矩阵作为参数来构造分解对象。作为示例，让我们考虑partial pivoting的LU分解。

            //声明一个2x2矩阵 A:

            MatrixXd A(2, 2);
            A << 2, -1, 1, 3;
            cout << "Here is the input matrix A before decomposition:\n"
                 << A << endl;
            // Output is:
            // Here is the input matrix A before decomposition:
            //  2 -1
            //  1  3

            //毫不奇怪！然后，声明我们的Inplace LU对象lu，并检查矩阵的内容A:
            // 这相当于把A和lu绑定了，所以后文中即使计算A1，A1的内容也不会改变
            PartialPivLU<Ref<MatrixXd>> lu(A);
            cout << "Here is the input matrix A after decomposition:\n"
                 << A << endl;

            // Output is:
            // Here is the input matrix A after decomposition:
            //   2  -1
            // 0.5 3.5

            // 在这里，lu对象计算并将L和U因数存储在矩阵所保存的内存中A。
            //因此，系数A在分解过程中已被破坏，并由L和U因子代替，可以验证：
            // 也就是分解的结果保存在矩阵中
            cout << "Here is the matrix storing the L and U factors:\n"
                 << lu.matrixLU() << endl;
            // Output is:
            // Here is the matrix storing the L and U factors:
            //   2  -1
            // 0.5 3.5
            //然后，该lu对象可以像往常一样使用，例如解决Ax = b问题：
            MatrixXd A0(2, 2);
            A0 << 2, -1, 1, 3;
            VectorXd b(2);
            b << 1, 2;
            VectorXd x = lu.solve(b);
            cout << "Residual: " << (A0 * x - b).norm() << endl;
            // Output is:
            //Residual: 0
            //在这里，由于原始矩阵的内容A已丢失，因此我们必须声明一个新矩阵A0以验证结果。

            //由于在A和lu之间共享内存，因此修改矩阵A将导致lu无效。
            //可以通过修改的内容A并尝试再次解决初始问题来轻松验证这一点：
            A << 3, 4, -2, 1;
            x = lu.solve(b);
            // Output is:
            //Residual: 15.8114
            //请注意，这没有共享指针，所以保持输入矩阵A和lu具有共同的生命周期是用户的责任

            //如果要使用修改后的A更新因式分解，则必须像往常一样调用compute方法：
            A0 = A; // save A
            lu.compute(A);
            x = lu.solve(b);
            cout << "Residual: " << (A0 * x - b).norm() << endl;
            // Output is:
            //Residual: 0

            //请注意，调用compute不会更改该lu对象引用的内存。
            //因此，如果使用A1不同于的另一个矩阵调用计算方法A，
            //则A1不会修改内容。这仍然A是将用于存储矩阵的L和U因子的内容A1。可以很容易地验证如下：
            MatrixXd A1(2, 2);
            A1 << 5, -2, 3, 4;
            lu.compute(A1);
            cout << "Here is the input matrix A1 after decomposition:\n"
                 << A1 << endl;
            // Output is:
            // Here is the input matrix A1 after decomposition:
            //  5 -2
            //  3  4

            //矩阵A1是不变的，因此可以求解A1 *x = b，直接检查残差而无需任何副本A1 :
            x = lu.solve(b);
            cout << "Residual: " << (A1 * x - b).norm() << endl;
            // Output is:
            //Residual: 2.48253e-16

            //这是支持此就位机制的矩阵分解列表：
            // class LLT
            // class LDLT
            // class PartialPivLU
            // class FullPivLU
            // class HouseholderQR
            // class ColPivHouseholderQR
            // class FullPivHouseholderQR
            // class CompleteOrthogonalDecomposition
        }

    } // namespace Section4_InplaceDecompisitions
    //complete pivoting和partial pivoting
    // （1） complete pivoting（即full pivoting），就是在矩阵分解或高斯消元的过程中，主元是在未进行变换的所有行和列之间进行选择。也就是需要同时进行行交换和列交换。
    // （2） partial pivoting就是只在当前进行变换的列中选择主元，只需要进行行交换
    namespace Section5_BenchmarkOfDenseDecompositions {

        // 本页介绍了Eigen针对各种方阵和过约束问题提供的稠密矩阵分解的速度比较。

        // 有关线性求解器和分解的特征和数值鲁棒性的更一般的概述，请查看此表。http://eigen.tuxfamily.org/dox/group__TopicLinearAlgebraDecompositions.html

        // 该基准测试已在配备Intel Core i7 @ 2,6 GHz的笔记本电脑上运行，
        //并使用启用了AVX和FMA指令集但没有多线程的clang进行编译。它使用单 精度 浮点数。对于双精度，您可以通过将时序乘以2来获得良好的估计。

        // 方矩阵是对称的，对于过度约束的矩阵，报告的时间包括$ A ^ TA $基于Cholesky和LU为前四个求解器计算对称协方差矩阵的成本，如*符号所示（表的右上角部分） ）。
        //时间以毫秒为单位，并且因数是相对于LLT分解而言的，这是最快的，但也是最不通用且最不可靠的。

        // 求解器/大小	8x8	100x100	1000x1000	4000x4000	10000x8	10000x100	10000x1000	10000x4000
        // LLT	0.05	0.42	5.83	374.55	6.79 *	30.15 *	236.34 *	3847.17 *
        // LDLT	0.07（x1.3）	0.65（x1.5）	26.86（x4.6）	2361.18（x6.3）	6.81（x1）*	31.91（x1.1）*	252.61（x1.1）*	5807.66（x1.5）*
        // PartialPivLU	0.08（x1.5）	0.69（x1.6）	15.63（x2.7）	709.32（x1.9）	6.81（x1）*	31.32（x1）*	241.68（x1）*	4270.48（x1.1）*
        // FullPivLU	0.1（x1.9）	4.48（x10.6）	281.33（x48.2）	--	6.83（x1）*	32.67（x1.1）*	498.25（x2.1）*	--
        // HouseholderQR	0.19（x3.5）	2.18（x5.2）	23.42（x4）	1337.52（x3.6）	34.26（x5）	129.01（x4.3）	377.37（x1.6）	4839.1（x1.3）
        // ColPivHouseholderQR	0.23（x4.3）	2.23（x5.3）	103.34（x17.7）	9987.16（x26.7）	36.05（x5.3）	163.18（x5.4）	2354.08（x10）	37860.5（x9.8）
        // CompleteOrthogonalDecomposition	0.23（x4.3）	2.22（x5.2）	99.44（x17.1）	10555.3（x28.2）	35.75（x5.3）	169.39（x5.6）	2150.56（x9.1）	36981.8（x9.6）
        // FullPivHouseholderQR	0.23（x4.3）	4.64（x11）	289.1（x49.6）	--	69.38（x10.2）	446.73（x14.8）	4852.12（x20.5）	--
        // JacobiSVD	1.01（x18.6）	71.43（x168.4）	--	--	113.81（x16.7）	1179.66（x39.1）	--	--
        // BDCSVD	1.07（x19.7）	21.83（x51.5）	331.77（x56.9）	18587.9（x49.6）	110.53（x16.3）	397.67（x13.2）	2975（x12.6）	48593.2（x12.6）
        // *：此分解不支持对过度约束的问题进行直接最小二乘法求解，并且所报告的时间包括形成对称协方差矩阵的成本$ A ^ TA $。

        // 观察结果：

        // LLT始终是最快的解决方案。
        // 对于很大程度上过度约束的问题，Cholesky / LU分解的成本主要由对称协方差矩阵的计算决定。
        // 对于较大的问题，只有实现高速缓存友好的分块策略的分解才能很好地进行扩展。
        //这些包括LLT，PartialPivLU，HouseholderQR和BDCSVD。这解释了为什么对于4k x 4k矩阵，HouseholderQR比LDLT更快。将来，LDLT和ColPivHouseholderQR也将实施分块策略。
        // CompleteOrthogonalDecomposition基于ColPivHouseholderQR，因此可以达到相同的性能水平。
        // 上表是由bench / dense_solvers.cpp文件生成的，可以随意修改以生成与您的硬件，编译器和喜欢的问题大小相匹配的表。

    } // namespace Section5_BenchmarkOfDenseDecompositions
} // namespace Chapter2_DenseLinearProblemsAndDecompositions
void PrintMsg(int i)
{
    cout << endl
         << "*******  Results   of   Chapter2   Section" << i << "  ********" << endl;
}

void TestChapter2Section1()
{
    PrintMsg(1);
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::BasicLinearSolving();
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::CheckingIfASolutionReallyExists();
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::ComputingEigenvaluesAndEigenvectors();
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::ComputingInverseAndDeterminant();
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::LeastSquaresSolving();
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::RankRevealingDecompositions();
    Chapter2_DenseLinearProblemsAndDecompositions::Section1_LinearAlgebraAndDecompositions::SeparatingTheComputationFromTheConstruction();
}

void TestChapter2Section2()
{
    //这部分没有程序，主要讲解稠密矩阵的分解
}

void TestChapter2Section3()
{
    PrintMsg(3);
    //讨论的三种方法是SVD分解，QR分解和正态方程。
    //其中，SVD分解通常最准确但最慢，正则方程(normal equations)最快但最不准确，QR分解介于两者之间。
    Chapter2_DenseLinearProblemsAndDecompositions::Section3_SolvingLinearLeastSquaresSystems::UsingNormalEquations();
    Chapter2_DenseLinearProblemsAndDecompositions::Section3_SolvingLinearLeastSquaresSystems::UsingTheQRDecomposition();
    Chapter2_DenseLinearProblemsAndDecompositions::Section3_SolvingLinearLeastSquaresSystems::UsingTheSvdDecomposition();
}
void TestChapter2Section4()
{
    PrintMsg(4);
    Chapter2_DenseLinearProblemsAndDecompositions::Section4_InplaceDecompisitions::InplaceMatrixDecompositions();
}
void TestChapter2Section5()
{
    //这部分没有程序，介绍了针对各种方阵和过约束问题提供的稠密矩阵分解的速度比较。
}
int main() {
    // build pass
    TestChapter2Section1();

    // build pass
    TestChapter2Section3();

    // build pass
    TestChapter2Section4();
    return 0;
}