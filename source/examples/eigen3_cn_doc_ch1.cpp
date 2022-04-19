/**
 * https://github.com/qixianyu-buaa/EigenChineseDocument/tree/master/Eigen/Chapter1_DenseMatrixAndArrary
 */
#include <iostream>
// The Eigen/Dense header file defines all member functions for the MatrixXd type and related types
#include <Eigen/Dense>
//All classes and functions defined in this header file (and other Eigen header files) are in the Eigen namespace.

#define LOG()  printf("\n=========  %s() Output is  ========= \n",__FUNCTION__);

using namespace Eigen;
using namespace std;

namespace Chapter1_DenseMatrixAndArrary {

    //http://eigen.tuxfamily.org/dox/group__TutorialMatrixClass.html
    namespace Section1_MatrixClass {

        // Eigen中的matrix和vector底层采用Matrix<>模板类表示，因为向量是特殊的矩阵，例如行向量是一行多列，列向量是一列多行
        //  Matrix<>模板类定义：
        // ``` Matrix<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
        //``` typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
        //``` typedef Matrix<int, Dynamic, 1> VectorXi;

        //```Eigen中的MatrixXt基本上是动态内存分配和初始化分开

        //```固定大小的向量提供了初始化操作
        //               Vector2d a(5.0, 6.0);
        //               Vector3d b(5.0, 6.0, 7.0);
        //               Vector4d c(5.0, 6.0, 7.0, 8.0);

        // 获取元素：
        // 所有元素：m(index)的方式可以获取数据，但是和存储方式有关，
        // eigen默认采用column-major storage order, but this can be changed to row-major, 注意，存储顺序与逗号初始化是无关系的
        // []运算符只能获取**向量元素**，因为C++[]只支持一个参数；

        void CoefficientAccessors() {
            LOG();
            // MatrixXd type represents a matrix of arbitrary size (hence the X in MatrixXd), in which every entry is a double (hence the d in MatrixXd)
            // Declares a variable of type MatrixXd and specifies that it is a matrix with 2 rows and 2 columns (the entries are not initialized).
            // 定义矩阵大小，但未初始化,该方式在堆上分配内存
            MatrixXd m(2, 2);

            //The statement m(0,0) = 3 sets the entry in the top-left corner to 3. You need to use round parentheses to refer to entries in the matrix.
            //As usual in computer science, the index of the first index is 0, as opposed to the convention in mathematics that the first index is 1.
            // 矩阵元素赋值，index 从0开始
            m(0, 0) = 3;
            m(1, 0) = 2.5;
            m(0, 1) = -1;
            m(1, 1) = m(1, 0) + m(0, 1);
            std::cout << m << std::endl;
            // Output is:
            // Here is the matrix m:
            //   3  -1
            // 2.5 1.5
            // Here is the vector v:
            // 4
            // 3
        }

        void CommaInitialization() {
            LOG();
            //矩阵和矢量系数可以使用所谓的逗号初始化语法方便地设置。现在，只需要知道以下示例即可：
            Matrix3f m;
            m << 1, 2, 3,
                    4, 5, 6,
                    7, 8, 9;
            std::cout << m;
        }

        void Resizing() {
            LOG();
            //矩阵的当前大小可以通过rows()，cols()和size()获取。
            //这些方法分别返回行数，列数和系数数。调整动态大小矩阵的大小是通过resize()方法完成的。
            // 如果实际矩阵大小不变，则resize()方法为空操作。
            // 否则具有破坏性：系数的值可能会更改。如果你想调整大小()，它不改变系数，使用conservativeResize()

            MatrixXd m(2, 5);
            m.resize(4, 3);
            std::cout << "The matrix m is of size "
                      << m.rows() << "x" << m.cols() << std::endl;
            std::cout << "It has " << m.size() << " coefficients" << std::endl;
            VectorXd v(2);
            v.resize(5);
            std::cout << "The vector v is of size " << v.size() << std::endl;
            std::cout << "As a matrix, v is of size "
                      << v.rows() << "x" << v.cols() << std::endl;
            // Output is:
            // The matrix m is of size 4x3
            // It has 12 coefficients
            // The vector v is of size 5
            // As a matrix, v is of size 5x1

            {
                //为了实现API统一性，所有这些方法仍可用于固定大小的矩阵。当然，您实际上不能调整固定大小的矩阵的大小。尝试将固定大小更改为实际不同的值将触发断言失败。
                //但是以下代码是合法的：
                Matrix4d m;
                m.resize(4, 4); // no operation
                std::cout << "The matrix m is of size "
                          << m.rows() << "x" << m.cols() << std::endl;
            }
        }

        void MatrixAndVectorRunTime() {
            LOG();
            // 固定大小的矩阵or向量在栈上分配内存，因为它在编译时期就可以确定大小
            // Matrix4f mymatrix  ≈  float mymatrix[16];
            // MatrixX表示运行时才确定矩阵的大小，因为它在堆上分配内存
            // MatrixXf mymatrix(rows,columns) ≈  float *mymatrix = new float[rows*columns];
            MatrixXd m = MatrixXd::Random(3, 3);          //  random values between -1 and 1
            m = (m + MatrixXd::Constant(3, 3, 1.2)) *
                50; // MatrixXd::Constant(3, 3, 1.2) represents a 3-by-3 matrix expression having all coefficients equal to 1.2
            cout << "m =" << endl
                 << m << endl;
            VectorXd v(3); // 尚未初始化
            v << 1, 2, 3;  //  uses the so-called comma-initializer
            cout << "m * v =" << endl
                 << m * v << endl;
        }

        void MatrixAndVectorCompileTime() {
            LOG();
            // 编译时可确定矩阵尺寸，在栈上分配内存
            Matrix3d m = Matrix3d::Random(); //a fixed size
            m = (m + Matrix3d::Constant(1.2)) * 50;
            cout << "m =" << endl
                 << m << endl;
            Vector3d v(1, 2, 3);

            cout << "m * v =" << endl
                 << m * v << endl;
        }

        void AssignmentAndResizing() {
            LOG();
            //operator= 将矩阵复制到另一个矩阵中的操作。Eigen自动调整左侧矩阵的大小，使其与右侧大小的矩阵大小匹配。例如：
            MatrixXf a(2, 2);
            std::cout << "a is of size " << a.rows() << "x" << a.cols() << std::endl;
            MatrixXf b(3, 3);
            a = b;
            std::cout << "a is now of size " << a.rows() << "x" << a.cols() << std::endl;

        }

        void FixedSizeAndDynamicSize() {

            LOG();
            //固定尺寸与动态尺寸
            // 什么时候应该使用固定尺寸（例如Matrix4f），什么时候应该使用动态尺寸（例如MatrixXf）？
            //简单的答案是：在可能的地方使用固定尺寸来显示非常小的尺寸，在需要的地方使用动态尺寸来显示较大的尺寸。
            //对于小尺寸，尤其是对于小于（大约）16的尺寸，使用固定尺寸对性能有极大的好处，因为它使Eigen避免了动态内存分配并展开了循环。
            //在内部，固定大小的本征矩阵只是一个简单的数组，即
            // Matrix4f mymatrix;
            // 真的等于只是在做
            // float[16];
            // 因此这确实具有零运行时间成本。相比之下，动态大小矩阵的数组始终分配在堆上，因此

            // MatrixXf mymatrix（行，列）;
            // 等于做
            // float * mymatrix = new [行*列];
            // 除此之外，MatrixXf对象还将其行数和列数存储为成员变量。
            // 当然，使用固定大小的限制是，只有当您在编译时知道大小时，才有可能这样做。
            //同样，对于足够大的尺寸（例如，对于大于（大约）32的尺寸），使用固定尺寸的性能优势变得可以忽略不计。
            //更糟糕的是，尝试使用函数内部的固定大小创建非常大的矩阵可能会导致堆栈溢出，因为Eigen会尝试自动将数组分配为局部变量，而这通常是在堆栈上完成的。
            //最后，视情况而定，当使用动态尺寸时，Eigen还可尝试进行矢量化（使用SIMD指令），请参见参考矢量化。
        }

        void OptionalTemplateParameters() {
            LOG();
            // 可选模板参数
            // 我们在页面开始时提到Matrix类采用六个模板参数，但到目前为止，我们仅讨论了前三个。其余三个参数是可选的。这是模板参数的完整列表：

            //       Matrix<typename Scalar,
            //        int RowsAtCompileTime,
            //        int ColsAtCompileTime,
            //        int Options = 0,
            //        int MaxRowsAtCompileTime = RowsAtCompileTime,
            //        int MaxColsAtCompileTime = ColsAtCompileTime>

            // Options是位字段。在这里，我们只讨论一点：RowMajor。它指定这种类型的矩阵使用行优先存储顺序。默认情况下，存储顺序为“按列的顺序存储”。
            //请参阅有关存储顺序的页面。例如，此类型表示行优先存储的3x3矩阵：
            // Matrix<float，3、3，RowMajor>
            // MaxRowsAtCompileTime并且MaxColsAtCompileTime在您希望指定时很有用，即使在编译时不知道矩阵的确切大小，在编译时也知道固定的上限。
            //您可能要这样做的最大原因是避免动态内存分配。例如，以下矩阵类型使用12个浮点数的普通数组，而不分配动态内存：
            // Matrix<float，Dynamic，Dynamic，0、3、4>
        }

        void ConvenienceTypedefs() {
            LOG();
            // Eigen定义了以下Matrix typedef：

            // MatrixNt for Matrix<type, N, N>. For example, MatrixXi for Matrix<int, Dynamic, Dynamic>.
            // VectorNt for Matrix<type, N, 1>. For example, Vector2f for Matrix<float, 2, 1>.
            // RowVectorNt for Matrix<type, 1, N>. For example, RowVector3d for Matrix<double, 1, 3>.
            // Where:
            // N可以是任何一个2，3，4，或X（意思Dynamic）。
            // t可以是i（表示int），f（表示float），d（表示double），cf（表示complex <float>）或cd（表示complex <double>）的任何一种。
            //仅针对这五个类型定义typedef的事实并不意味着它们是唯一受支持的标量类型。例如，支持所有标准整数类型，请参阅标量类型。
        }

    } // namespace Section1_MatrixClass
    namespace Section2_MatrixAndVectorArithmetic {

        // 本部分介绍了矩阵在线性代数中的基本运算，相信学过线性代数的同学理解起来十分容易
        // 需要注意的是：
        //      1)对于a = a.transpose()操作，应该使用a.transposeInPlace(),  见TranspositionAndConjugation()
        //      2)向量叉积只适用于大小为3的向量，点积适用于任意向量, 见DotProductAndCrossProduct()
        //      3)理解矩阵Reduction操作的意义

        void AdditionAndSubtraction() {
            LOG();
            //binary operator + as in a+b
            //binary operator - as in a-b
            //unary operator - as in -a
            //compound operator += as in a+=b
            //compound operator -= as in a-=b
            Matrix2d a;
            a << 1, 2,
                    3, 4;
            MatrixXd b(2, 2);
            b << 2, 3,
                    1, 4;
            std::cout << "a + b =\n"
                      << a + b << std::endl;
            std::cout << "a - b =\n"
                      << a - b << std::endl;
            std::cout << "Doing a += b;" << std::endl;
            a += b;
            std::cout << "Now a =\n"
                      << a << std::endl;
            Vector3d v(1, 2, 3);
            Vector3d w(1, 0, 0);
            std::cout << "-v + w - v =\n"
                      << -v + w - v << std::endl;
            //Output is:
            // a + b =
            // 3 5
            // 4 8
            // a - b =
            // -1 -1
            //  2  0
            // Doing a += b;
            // Now a =
            // 3 5
            // 4 8
            // -v + w - v =
            // -1
            // -4
            // -6
        }

        void ScalarMultiplicationAndDivision() {
            LOG();
            // binary operator * as in matrix*scalar
            // binary operator * as in scalar*matrix
            // binary operator / as in matrix/scalar
            // compound operator *= as in matrix*=scalar
            // compound operator /= as in matrix/=scalar
            Matrix2d a;
            a << 1, 2,
                    3, 4;
            Vector3d v(1, 2, 3);
            std::cout << "a * 2.5 =\n"
                      << a * 2.5 << std::endl;
            std::cout << "0.1 * v =\n"
                      << 0.1 * v << std::endl;
            std::cout << "Doing v *= 2;" << std::endl;
            v *= 2;
            std::cout << "Now v =\n"
                      << v << std::endl;
            //Output is:
            //  a * 2.5 =
            // 2.5   5
            // 7.5  10
            // 0.1 * v =
            // 0.1
            // 0.2
            // 0.3
            // Doing v *= 2;
            // Now v =
            // 2
            // 4
            // 6
        }

        void ANoteAboutExpressionTemplates() {
            LOG();
            // 在Eigen中，诸如算术运算符（例如）operator+自己并不执行任何计算,
            // 它们仅返回描述要执行的计算的“表达式对象”。当计算整个表达式时，实际的计算将在稍后进行，
            // 通常在中operator=。尽管这听起来很沉重，但是任何现代的优化编译器都可以优化该抽象，从而获得完美优化的代码。例如，当您这样做时：

            // VectorXf a(50), b(50), c(50), d(50);
            // ...
            // a = 3*b + 4*c + 5*d;
            // Eigen将其编译为一个for循环，因此数组仅被遍历一次。简化（例如忽略SIMD优化），此循环如下所示：

            // 对于（int i = 0; i <50; ++ i）
            //   a [i] = 3 * b [i] + 4 * c [i] + 5 * d [i]；

            // 因此，您不必担心Eigen使用相对较大的算术表达式：它只会为Eigen提供更多优化机会。
        }

        void TranspositionAndConjugation() {

            LOG();
            //矩阵或向量的转置$ a ^ T $，共轭$ \ bar {a} $和伴随（即共轭转置）分别通过成员函数transpose（），conjugate（）和adjoint（）获得
            {
                MatrixXcf a = MatrixXcf::Random(2, 2); //MatrixXcf 为复数矩阵
                cout << "Here is the matrix a\n"
                     << a << endl;
                cout << "Here is the matrix a^T\n"
                     << a.transpose() << endl;
                cout << "Here is the conjugate of a\n"
                     << a.conjugate() << endl;
                cout << "Here is the matrix a^*\n"
                     << a.adjoint() << endl;

                //Output is:
                //   Here is the matrix a
                //  (-0.211,0.68) (-0.605,0.823)
                //  (0.597,0.566)  (0.536,-0.33)
                // Here is the matrix a^T
                //  (-0.211,0.68)  (0.597,0.566)
                // (-0.605,0.823)  (0.536,-0.33)
                // Here is the conjugate of a
                //  (-0.211,-0.68) (-0.605,-0.823)
                //  (0.597,-0.566)    (0.536,0.33)
                // Here is the matrix a^*
                //  (-0.211,-0.68)  (0.597,-0.566)
                // (-0.605,-0.823)    (0.536,0.33)
            }

            // 注意：对于一个矩阵自身的转置，应该使用.transposeInPlace()
            {
                Matrix2i a;
                a << 1, 2, 3, 4;
                cout << "Here is the matrix a:\n"
                     << a << endl;
                // 坑～～～～～～不应该这样～～～～～～～～
                // a = a.transpose(); // !!! do NOT do this !!!
                // cout << "and the result of the aliasing effect:\n"
                //      << a << endl;

                // 应该这样～～～～
                a.transposeInPlace();
                cout << "and after being transposed:\n"
                     << a << endl;
                //Output is:
                // Here is the initial matrix a:
                // 1 2 3
                // 4 5 6
                // and after being transposed:
                // 1 4
                // 2 5
                // 3 6
            }
        }

        void MatrixMatrixAndMatrixVectorMultiplication() {
            LOG();
            Matrix2d mat;
            mat << 1, 2,
                    3, 4;
            Vector2d u(-1, 1), v(2, 0);
            std::cout << "Here is mat*mat:\n"
                      << mat * mat << std::endl;
            std::cout << "Here is mat*u:\n"
                      << mat * u << std::endl;
            std::cout << "Here is u^T*mat:\n"
                      << u.transpose() * mat << std::endl;
            std::cout << "Here is u^T*v:\n"
                      << u.transpose() * v << std::endl;
            std::cout << "Here is u*v^T:\n"
                      << u * v.transpose() << std::endl;
            std::cout << "Let's multiply mat by itself" << std::endl;
            mat = mat * mat;
            std::cout << "Now mat is mat:\n"
                      << mat << std::endl;

            //Output is:
            // Here is mat*mat:
            //  7 10
            // 15 22
            // Here is mat*u:
            // 1
            // 1
            // Here is u^T*mat:
            // 2 2
            // Here is u^T*v:
            // -2
            // Here is u*v^T:
            // -2 -0
            //  2  0
            // Let's multiply mat by itself
            // Now mat is mat:
            //  7 10
            // 15 22
        }

        void DotProductAndCrossProduct() {
            LOG();
            //  cross product is only for vectors of size 3. Dot product is for vectors of any sizes
            // 叉积只适用于大小为3的向量，点积适用于任意向量
            Vector3d v(1, 2, 3);
            Vector3d w(0, 1, 2);
            cout << "Dot product: " << v.dot(w) << endl;
            double dp = v.adjoint() * w; // automatic conversion of the inner product to a scalar
            cout << "Dot product via a matrix product: " << dp << endl;
            cout << "Cross product:\n"
                 << v.cross(w) << endl;
            // Output is:
            // Dot product: 8
            // Dot product via a matrix product: 8
            // Cross product:
            //  1
            // -2
            //  1
        }

        void BasicArithmeticReductionOperations() {
            LOG();
            // Eigen also provides some reduction operations to reduce a given matrix or vector to a single value
            // such as the sum (computed by sum()), product (prod()), or the maximum (maxCoeff()) and minimum (minCoeff()) of all its coefficients.
            Eigen::Matrix2d mat;
            mat << 1, 2,
                    3, 4;
            //元素和，元素乘积，元素均值，最小系数，最大系数，踪
            cout << "Here is mat.sum():       " << mat.sum() << endl;
            cout << "Here is mat.prod():      " << mat.prod() << endl;
            cout << "Here is mat.mean():      " << mat.mean() << endl;
            cout << "Here is mat.minCoeff():  " << mat.minCoeff() << endl;
            cout << "Here is mat.maxCoeff():  " << mat.maxCoeff() << endl;
            cout << "Here is mat.trace():     " << mat.trace() << endl;

            // 可以返回元素位置
            Matrix3f m = Matrix3f::Random();
            std::ptrdiff_t i, j;
            float minOfM = m.minCoeff(&i, &j);
            cout << "Here is the matrix m:\n"
                 << m << endl;
            cout << "Its minimum coefficient (" << minOfM
                 << ") is at position (" << i << "," << j << ")\n\n";
            RowVector4i v = RowVector4i::Random();
            int maxOfV = v.maxCoeff(&i);
            cout << "Here is the vector v: " << v << endl;
            cout << "Its maximum coefficient (" << maxOfV
                 << ") is at position " << i << endl;
            // Output is:
            // Here is mat.sum():       10
            // Here is mat.prod():      24
            // Here is mat.mean():      2.5
            // Here is mat.minCoeff():  1
            // Here is mat.maxCoeff():  4
            // Here is mat.trace():     5
            // Here is the matrix m:
            //  -0.444451   0.257742   0.904459
            //    0.10794  -0.270431    0.83239
            // -0.0452059  0.0268018   0.271423
            // Its minimum coefficient (-0.444451) is at position (0,0)
        }

        void ValidityOfoperations() {
            LOG();
            // Eigen的输出很操蛋，但是它把输出中关键的信息大写了～
            // 如果你想体验一下Eigen的错误输出，则注释掉下面的内容

            // Matrix3f m;
            // Vector4f v;
            // v = m * v; // Compile-time error: YOU_MIXED_MATRICES_OF_DIFFERENT_SIZES

            // {
            //         MatrixXf m(3, 3);
            //         VectorXf v(4);
            //         v = m * v; // Run-time assertion failure here: "invalid matrix product"
            // }
        }
    } // namespace Section2_MatrixAndVectorArithmetic
    // 与用于线性代数的Matrix类相反，Array类提供了通用数组。
    // 此外，Array类提供了一种执行逐系数运算的简便方法，该运算可能没有线性代数含义，
    // 例如将常数添加到数组中的每个系数或按系数乘两个数组。

    // Array<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>

    // Eigen还为某些常见情况提供了typedef，其方式类似于Matrix typedef，但有一些细微的差异
    // 因为单词“ array”既用于一维数组，也用于二维数组.

    //**一维数组**采用ArrayNt形式的typedef代表，其中N和t是大小和标量类型

    // **二维数组**采用ArrayNNt形式的typedef
    // Type	                                                        Typedef
    // Array<float,Dynamic,1>                   ArrayXf
    // Array<float,3,1>                                            Array3f
    // Array<double,Dynamic,Dynamic>        ArrayXXd
    // Array<double,3,3>                                            Array33d

    namespace Section3_ArrayAndCoefficientwiseOperations {

        void AccessingValuesInsideAnArray() {
            LOG();
            ArrayXXf m(2, 2); // 二维动态float类型数组

            // assign some values coefficient by coefficient
            m(0, 0) = 1.0;
            m(0, 1) = 2.0;
            m(1, 0) = 3.0;
            m(1, 1) = m(0, 1) + m(1, 0);

            // print values to standard output
            cout << m << endl
                 << endl;

            // using the comma-initializer is also allowed
            m << 1.0, 2.0,
                    3.0, 4.0;

            // print values to standard output
            cout << m << endl;

            // Output is:
            // 1 2
            // 3 5

            // 1 2
            // 3 4
        }

        void AdditionAndSubtraction() {
            // 加减法
            // 两个数组的加减法与矩阵相同。
            // 如果两个数组的大小相同，并且该加法或减法是按系数进行的，则此操作有效。
            // 数组还支持以下形式的表达式，该表达式array + scalar将标量添加到数组中的每个系数。
            // 这提供了不能直接用于Matrix对象的功能。
            LOG();
            ArrayXXf a(3, 3);
            ArrayXXf b(3, 3);
            a << 1, 2, 3,
                    4, 5, 6,
                    7, 8, 9;
            b << 1, 2, 3,
                    1, 2, 3,
                    1, 2, 3;

            // Adding two arrays
            cout << "a + b = " << endl
                 << a + b << endl
                 << endl;
            // Subtracting a scalar from an array
            cout << "a - 2 = " << endl
                 << a - 2 << endl;
            // Output is:
            // a + b =
            //  2  4  6
            //  5  7  9
            //  8 10 12

            // a - 2 =
            // -1  0  1
            //  2  3  4
            //  5  6  7
        }

        void ArrayMultiplication() {
            //您可以将一个数组乘以标量，这与矩阵的工作方式相同。
            //数组与矩阵根本不同的地方是将两个矩阵相乘。
            //矩阵将乘法解释为矩阵乘积，而数组将乘法解释为按系数乘积。
            //因此，当且仅当两个数组具有相同的维数时，它们才能相乘
            LOG();
            ArrayXXf a(2, 2);
            ArrayXXf b(2, 2);
            a << 1, 2,
                    3, 4;
            b << 5, 6,
                    7, 8;
            cout << "a * b = " << endl
                 << a * b << endl;
            // Output is:
            // a * b =
            //  5 12
            // 21 32
        }

        void OtherCoefficientwiseOperations() {
            // 数组类定义除上述加法，减法和乘法运算符其他系数为单位的运算。
            // 例如，.abs()方法获取每个系数的绝对值，而.sqrt（）计算系数的平方根。
            //如果您有两个大小相同的数组，则可以调用.min()来构造其系数是两个给定数组中对应系数的最小值的数组。
            //注意:.array()和.matrix() 转换为相同的维数，但是不同的对象具有不同的方法
            LOG();
            ArrayXf a = ArrayXf::Random(5);
            a *= 2;
            cout << "a =" << endl
                 << a << endl;
            cout << "a.abs() =" << endl
                 << a.abs() << endl;
            cout << "a.abs().sqrt() =" << endl
                 << a.abs().sqrt() << endl;
            cout << "a.min(a.abs().sqrt()) =" << endl
                 << a.min(a.abs().sqrt()) << endl;
            // Output is:
            // a =
            //   1.36
            // -0.422
            //   1.13
            //   1.19
            //   1.65
            // a.abs() =
            //  1.36
            // 0.422
            //  1.13
            //  1.19
            //  1.65
            // a.abs().sqrt() =
            // 1.17
            // 0.65
            // 1.06
            // 1.09
            // 1.28
            // a.min(a.abs().sqrt()) =
            //   1.17
            // -0.422
            //   1.06
            //   1.09
            //   1.28
        }

        void ConvertingBetweenArrayAndMatrixExpressions() {
            // 什么时候应该使用Matrix类的对象，什么时候应该使用Array类的对象？
            //您不能对数组应用矩阵运算，也不能对矩阵应用数组运算。
            //因此，如果您需要进行线性代数运算（例如矩阵乘法），则应使用矩阵。
            //如果需要进行系数运算，则应使用数组。
            //但是，有时并不是那么简单，但是您需要同时使用Matrix和Array操作。
            //在这种情况下，您需要将矩阵转换为数组或反向转换。无论选择将对象声明为数组还是矩阵，都可以访问所有操作。
            // 矩阵表达式具有.array()方法，可以将它们“转换”为数组表达式，因此可以轻松地应用按系数进行运算。
            //相反，数组表达式具有.matrix()方法。
            //与所有Eigen表达式抽象一样，这没有任何运行时开销（只要您让编译器进行优化）
            //.array（）和.matrix() 可被用作右值和作为左值。

            // Eigen禁止在表达式中混合矩阵和数组。
            // 例如，您不能直接矩阵和数组相加。运算符+的操作数要么都是矩阵，要么都是数组。
            //但是，使用.array（）和.matrix（）可以轻松地将其转换为另一种。

            //～～～～注意～～～～～
            //此规则的例外是赋值运算符=：允许将矩阵表达式分配给数组变量，或将数组表达式分配给矩阵变量。

            // 下面的示例演示如何通过使用.array（）方法对Matrix对象使用数组操作。
            // 例如，语句需要两个矩阵和，他们两个转换为阵列，用来将它们相乘系数明智并将结果指定给矩阵变量（这是合法的，因为本征允许分配数组表达式到矩阵的变量）。result = m.array() * n.array()mnresult

            // 实际上，这种使用情况非常普遍，以至于Eigen为矩阵提供了const .cwiseProduct()方法来计算系数乘积。
            LOG();
            MatrixXf m(2, 2);
            MatrixXf n(2, 2);
            MatrixXf result(2, 2);
            m << 1, 2,
                    3, 4;
            n << 5, 6,
                    7, 8;
            result = m * n;
            cout << "-- Matrix m*n: --" << endl
                 << result << endl
                 << endl;
            result = m.array() * n.array();
            cout << "-- Array m*n: --" << endl
                 << result << endl
                 << endl;
            result = m.cwiseProduct(n);
            cout << "-- With cwiseProduct: --" << endl
                 << result << endl
                 << endl;
            result = m.array() + 4;
            cout << "-- Array m + 4: --" << endl
                 << result << endl
                 << endl;

            // Output is:
            // -- Matrix m*n: --
            // 19 22
            // 43 50

            // -- Array m*n: --
            //  5 12
            // 21 32

            // -- With cwiseProduct: --
            //  5 12
            // 21 32

            // -- Array m + 4: --
            // 5 6
            // 7 8
        }
    } // namespace Section3_ArrayAndCoefficientwiseOperations
    namespace Section4_BlockOperations {
        //前言：
        //块是矩阵或数组的某一部分。块表达式既可以用作右值，也可以用作左值。
        // Block operation
        // Block of size (p,q), starting at (i,j)

        //Version constructing a  dynamic-size block expression
        // 起始位置+行列数
        // matrix.block(i,j,p,q);

        //Version constructing a fixed-size block expression
        // 模板参数为行列数，函数参数为起始位置
        // matrix.block<p,q>(i,j);

        //两种版本均可用于固定大小和动态大小的矩阵和数组。
        //这两个表达式在语义上是等效的。
        //唯一的区别是，如果块大小较小，则固定大小版本通常会为您提供更快的代码，但是需要在编译时知道此大小。

        void UsingBlockOperations() {
            LOG();
            Eigen::MatrixXf m(4, 4);
            m << 1, 2, 3, 4,
                    5, 6, 7, 8,
                    9, 10, 11, 12,
                    13, 14, 15, 16;
            cout << "Block in the middle" << endl;
            cout << m.block<2, 2>(1, 1) << endl
                 << endl;
            for (int i = 1; i <= 3; ++i) {
                cout << "Block of size " << i << "x" << i << endl;
                cout << m.block(0, 0, i, i) << endl
                     << endl;
            }

            // Output is:
            // Block in the middle
            //  6  7
            // 10 11

            // Block of size 1x1
            // 1

            // Block of size 2x2
            // 1 2
            // 5 6

            // Block of size 3x3
            //  1  2  3
            //  5  6  7
            //  9 10 11
        }

        // block也可以作为左值
        void BlockAsLeftValue() {
            LOG();
            Array22f m;
            m << 1, 2,
                    3, 4;
            Array44f a = Array44f::Constant(0.6);
            cout << "Here is the array a:" << endl
                 << a << endl
                 << endl;
            a.block<2, 2>(1, 1) = m; //可作为左值
            cout << "Here is now a with m copied into its central 2x2 block:" << endl
                 << a << endl
                 << endl;
            a.block(0, 0, 2, 3) = a.block(2, 1, 2, 3);
            cout << "Here is now a with bottom-right 2x3 block copied into top-left 2x2 block:" << endl
                 << a << endl
                 << endl;

            // Output is:
            // ere is the array a:
            // 0.6 0.6 0.6 0.6
            // 0.6 0.6 0.6 0.6
            // 0.6 0.6 0.6 0.6
            // 0.6 0.6 0.6 0.6

            // Here is now a with m copied into its central 2x2 block:
            // 0.6 0.6 0.6 0.6
            // 0.6   1   2 0.6
            // 0.6   3   4 0.6
            // 0.6 0.6 0.6 0.6

            // Here is now a with bottom-right 2x3 block copied into top-left 2x2 block:
            //   3   4 0.6 0.6
            // 0.6 0.6 0.6 0.6
            // 0.6   3   4 0.6
            // 0.6 0.6 0.6 0.6
        }

        //单独的列和行是块的特殊情况。Eigen提供了可以轻松解决它们的方法：.col（）和.row（）。
        void ColumnsAndRows() {
            LOG();
            Eigen::MatrixXf m(3, 3);
            m << 1, 2, 3,
                    4, 5, 6,
                    7, 8, 9;
            cout << "Here is the matrix m:" << endl
                 << m << endl;
            cout << "2nd Row: " << m.row(1) << endl;

            m.col(2) += 3 * m.col(0);
            cout << "After adding 3 times the first column into the third column, the matrix m is:\n";
            cout << m << endl;

            // Output is:
            // Here is the matrix m:
            // 1 2 3
            // 4 5 6
            // 7 8 9
            // 2nd Row: 4 5 6
            // After adding 3 times the first column into the third column, the matrix m is:
            //  1  2  6
            //  4  5 18
            //  7  8 30
        }

        void CornerRelatedOperations() {
            LOG();
            //一系列矩阵的边角块操作
            // Block operation	Version constructing a dynamic-size block expression	Version constructing a fixed-size block expression
            // Top-left p by q block *	                                      matrix.topLeftCorner(p,q);                            matrix.topLeftCorner<p,q>();
            // Bottom-left p by q block *	                                 matrix.bottomLeftCorner(p,q);                    matrix.bottomLeftCorner<p,q>();
            // Top-right p by q block *	                                    matrix.topRightCorner(p,q);                         matrix.topRightCorner<p,q>();
            // Bottom-right p by q block *	                               matrix.bottomRightCorner(p,q);                 matrix.bottomRightCorner<p,q>();
            // Block containing the first q rows *	                  matrix.topRows(q);                                           matrix.topRows<q>();
            // Block containing the last q rows *	                  matrix.bottomRows(q);                                  matrix.bottomRows<q>();
            // Block containing the first p columns *	     matrix.leftCols(p);                                              matrix.leftCols<p>();
            // Block containing the last q columns *	     matrix.rightCols(q);                                           matrix.rightCols<q>();
            Eigen::Matrix4f m;
            m << 1, 2, 3, 4,
                    5, 6, 7, 8,
                    9, 10, 11, 12,
                    13, 14, 15, 16;
            cout << "m.leftCols(2) =" << endl
                 << m.leftCols(2) << endl
                 << endl;
            cout << "m.bottomRows<2>() =" << endl
                 << m.bottomRows<2>() << endl
                 << endl;
            m.topLeftCorner(1, 3) = m.bottomRightCorner(3, 1).transpose();
            cout << "After assignment, m = " << endl
                 << m << endl;
            // Output is:
            // m.leftCols(2) =
            //  1  2
            //  5  6
            //  9 10
            // 13 14

            // m.bottomRows<2>() =
            //  9 10 11 12
            // 13 14 15 16

            // After assignment, m =
            //  8 12 16  4
            //  5  6  7  8
            //  9 10 11 12
            // 13 14 15 16
        }

        void BlockOperationsForVectors() {
            LOG();
            //同样，向量也有一系列类似于矩阵的边角块操作
            // Block operation	Version                                                       constructing adynamic-size block expression	                        Version constructing afixed-size block expression
            // Block containing the first n elements *	                                                    vector.head(n);                                                                   vector.head<n>();
            // Block containing the last n elements *	                                                    vector.tail(n);                                                                       vector.tail<n>();
            // Block containing n elements, starting at position i *                           vector.segment(i,n);                                                         vector.segment<n>(i);
            Eigen::ArrayXf v(6);
            v << 1, 2, 3, 4, 5, 6;
            cout << "v.head(3) =" << endl
                 << v.head(3) << endl
                 << endl;
            cout << "v.tail<3>() = " << endl
                 << v.tail<3>() << endl
                 << endl;
            v.segment(1, 4) *= 2;
            cout << "after 'v.segment(1,4) *= 2', v =" << endl
                 << v << endl;

            // Output is:
            // v.head(3) =
            // 1
            // 2
            // 3

            // v.tail<3>() =
            // 4
            // 5
            // 6

            // after 'v.segment(1,4) *= 2', v =
            //  1
            //  4
            //  6
            //  8
            // 10
            //  6
        }

    } // namespace Section4_BlockOperations
    // 本页讨论了几种初始化矩阵的高级方法。
    // 它提供了有关之前介绍的逗号初始化程序的更多详细信息。
    // 它还说明了如何获得特殊矩阵，例如单位矩阵和零矩阵。

    namespace Section5_AdvancedInitialization {

        void CommaInitializer() {
            LOG();
            // Eigen提供了一种逗号初始化器语法，该语法使用户可以轻松设置矩阵，向量或数组的所有系数
            // 简单地列出系数，开始在左上角，并从左至右，从顶部向底部移动。
            // 需要预先指定对象的大小。
            // 如果列出的系数太少或太多，就会报错。

            Matrix3f mat;
            mat << 1, 2, 3,
                    4, 5, 6,
                    7, 8, 9;

            std::cout << mat << endl;

            //此外，初始化列表的元素本身可以是向量或矩阵。
            //通常的用途是将向量或矩阵连接在一起。
            //例如，这是将两个行向量连接在一起的方法。
            //  注意：～～～～～
            //请记住，必须先设置大小，然后才能使用逗号初始化程序
            RowVectorXd vec1(3);
            vec1 << 1, 2, 3;
            std::cout << "vec1 = " << vec1 << std::endl;
            RowVectorXd vec2(4);
            vec2 << 1, 4, 9, 16;
            std::cout << "vec2 = " << vec2 << std::endl;
            RowVectorXd joined(7);
            joined << vec1, vec2;
            std::cout << "joined = " << joined << std::endl;

            // 类似分块矩阵的初始化方式
            MatrixXf matA(2, 2);
            matA << 1, 2, 3, 4;
            MatrixXf matB(4, 4);
            matB << matA, matA / 10, matA / 10, matA;
            std::cout << matB << std::endl;

            //  对矩阵的某一块赋值
            Matrix3f m;
            m.row(0) << 1, 2, 3;
            m.block(1, 0, 2, 2) << 4, 5, 7, 8;
            m.col(2).tail(2) << 6, 9;
            std::cout << m;
        }

        void SpecialMatricesAndArrays() {
            LOG();
            // 模板类Matrix<>和Array<>有静态方法，可以帮助初始化；
            //有三种变体:
            //第一个变体不带参数，只能用于固定大小的对象。如果要将动态尺寸对象初始化为零，则需要指定尺寸。
            //第二个变体需要一个参数，并且可以用于一维动态尺寸对象，
            //第三个变体需要两个参数，并且可以用于二维对象。

            std::cout << "A fixed-size array:\n";
            Array33f a1 = Array33f::Zero();
            std::cout << a1 << "\n\n";
            std::cout << "A one-dimensional dynamic-size array:\n";
            ArrayXf a2 = ArrayXf::Zero(3);
            std::cout << a2 << "\n\n";
            std::cout << "A two-dimensional dynamic-size array:\n";
            ArrayXXf a3 = ArrayXXf::Zero(3, 4);
            std::cout << a3 << "\n";

            //同样，静态方法Constant(value)会将所有系数设置为value。
            // 如果需要指定对象的大小，则附加参数放在value参数之前，如
            // MatrixXd::Constant(rows, cols, value)。

            //Random()用随机系数填充矩阵或数组。

            //Identity()获得单位矩阵, 此方法仅适用于Matrix，不适用于Array，因为“单位矩阵”是线性代数概念。

            //该方法LinSpaced（尺寸，低，高）是仅可用于载体和一维数组; 它产生一个指定大小的向量，其系数在low和之间平均间隔high。
            //方法LinSpaced()以下示例说明了该示例，该示例打印一张表格，其中包含以度为单位的角度，以弧度为单位的相应角度以及它们的正弦和余弦值。
            ArrayXXf table(10, 4);
            table.col(0) = ArrayXf::LinSpaced(10, 0, 90);
            table.col(1) = M_PI / 180 * table.col(0);
            table.col(2) = table.col(1).sin();
            table.col(3) = table.col(1).cos();
            std::cout << "  Degrees   Radians      Sine    Cosine\n";
            std::cout << table << std::endl;

            //Eigen定义了诸如setZero()，MatrixBase :: setIdentity（）和DenseBase :: setLinSpaced()之类的实用程序函数来方便地执行此操作。
            //即，可以采用对象的成员函数设置初始值。

            //下面的示例对比了三种构造矩阵的J =[O  I ; I O ] 方法
            // 使用静态方法和operator=
            const int size = 6;
            MatrixXd mat1(size, size);
            mat1.topLeftCorner(size / 2, size / 2) = MatrixXd::Zero(size / 2, size / 2);
            mat1.topRightCorner(size / 2, size / 2) = MatrixXd::Identity(size / 2, size / 2);
            mat1.bottomLeftCorner(size / 2, size / 2) = MatrixXd::Identity(size / 2, size / 2);
            mat1.bottomRightCorner(size / 2, size / 2) = MatrixXd::Zero(size / 2, size / 2);
            std::cout << mat1 << std::endl
                      << std::endl;
            //使用.setXxx()方法
            MatrixXd mat2(size, size);
            mat2.topLeftCorner(size / 2, size / 2).setZero();
            mat2.topRightCorner(size / 2, size / 2).setIdentity();
            mat2.bottomLeftCorner(size / 2, size / 2).setIdentity();
            mat2.bottomRightCorner(size / 2, size / 2).setZero();
            std::cout << mat2 << std::endl
                      << std::endl;
            MatrixXd mat3(size, size);

            //使用静态方法和逗号初始化
            mat3 << MatrixXd::Zero(size / 2, size / 2), MatrixXd::Identity(size / 2, size / 2),
                    MatrixXd::Identity(size / 2, size / 2), MatrixXd::Zero(size / 2, size / 2);
            std::cout << mat3 << std::endl;
        }

        void UsageAsTemporaryObjects() {
            LOG();
            //如上所示，可以在声明时或在赋值运算符的右侧使用静态方法Zero()和Constant()来初始化变量。
            //您可以将这些方法视为返回矩阵或数组。
            //实际上，它们返回所谓的**表达式对象**，这些表达式对象在需要时求值到矩阵或数组，因此该语法不会产生任何开销。
            //这些表达式也可以用作临时对象。

            MatrixXd m = MatrixXd::Random(3, 3);
            m = (m + MatrixXd::Constant(3, 3, 1.2)) * 50;
            cout << "m =" << endl
                 << m << endl;
            VectorXd v(3);
            v << 1, 2, 3;
            cout << "m * v =" << endl
                 << m * v << endl;

            // The comma-initializer, too, can also be used to construct temporary objects.
            //The following example constructs a random matrix of size 2-by-3,
            // and then multiplies this matrix on the left with [0 1; 1 0]
            MatrixXf mat = MatrixXf::Random(2, 3);
            std::cout << mat << std::endl
                      << std::endl;
            //
            // The finished() method is necessary here to get the actual matrix object
            //  once the comma initialization of our temporary submatrix is done.
            //  finished 类似于endl，让它立即生成
            mat = (MatrixXf(2, 2) << 0, 1, 1, 0).finished() * mat;
            std::cout << mat << std::endl;
        }

    } // namespace Section5_AdvancedInitialization
    namespace Section6_ReductionsVisitorsBroadcasting {

        void Reductions() {
            LOG();
            //在Eigen中，约简是一个采用矩阵或数组并返回单个标量值的函数。最常用的归约方法之一是.sum（），它返回给定矩阵或数组内所有系数的总和。
            Eigen::Matrix2d mat;
            mat << 1, 2,
                    3, 4;
            cout << "Here is mat.sum():       " << mat.sum() << endl;
            cout << "Here is mat.prod():      " << mat.prod() << endl;
            cout << "Here is mat.mean():      " << mat.mean() << endl;
            cout << "Here is mat.minCoeff():  " << mat.minCoeff() << endl;
            cout << "Here is mat.maxCoeff():  " << mat.maxCoeff() << endl;
            cout << "Here is mat.trace():     " << mat.trace() << endl;
        }

        void NormComputations() {
            LOG();
            // 2范数的平方可以通过squaredNorm()获得。它本身等于矢量的点积，并且等效于其系数的平方绝对值的总和。
            //  norm（）方法返回squaredNorm（）的平方根。
            // 这些运算也可以在矩阵上运算。在这种情况下，n×p矩阵被视为大小（n * p）的向量，
            // 因此，例如norm()方法返回“ Frobenius”或“ Hilbert-Schmidt”范数。
            // 我们避免谈论l^2矩阵的范数，因为那可能意味着不同的事情。
            // 如果需要其他按系数分配的l^p范数，请使用lpNorm <p>()。
            // 如果需要无穷范数，则模板参数p可以采用特殊值Infinity，这是系数绝对值的最大值。
            VectorXf v(2);
            MatrixXf m(2, 2), n(2, 2);
            v << -1,
                    2;

            m << 1, -2,
                    -3, 4;
            // 向量范数
            cout << "v.squaredNorm() = " << v.squaredNorm() << endl;
            cout << "v.norm() = " << v.norm() << endl;
            cout << "v.lpNorm<1>() = " << v.lpNorm<1>() << endl;
            cout << "v.lpNorm<Infinity>() = " << v.lpNorm<Infinity>() << endl;
            cout << endl;
            // 矩阵范数
            cout << "m.squaredNorm() = " << m.squaredNorm() << endl;
            cout << "m.norm() = " << m.norm() << endl;
            cout << "m.lpNorm<1>() = " << m.lpNorm<1>() << endl;
            cout << "m.lpNorm<Infinity>() = " << m.lpNorm<Infinity>() << endl;

            // 也可自己计算1范数和无穷范数
            MatrixXf mat(2, 2);
            mat << 1, -2,
                    -3, 4;
            cout << "1-norm(mat)     = " << mat.cwiseAbs().colwise().sum().maxCoeff()
                 << " == " << mat.colwise().lpNorm<1>().maxCoeff() << endl;
            cout << "infty-norm(mat) = " << mat.cwiseAbs().rowwise().sum().maxCoeff()
                 << " == " << mat.rowwise().lpNorm<1>().maxCoeff() << endl;
        }

        void BooleanReductions() {
            LOG();
            // 矩阵或者数组皆有与，或，个数的方法
            // all() returns true if all of the coefficients in a given Matrix or Array evaluate to true .
            // any() returns true if at least one of the coefficients in a given Matrix or Array evaluates to true .
            // count() returns the number of coefficients in a given Matrix or Array that evaluate to true.
            ArrayXXf a(2, 2);

            a << 1, 2,
                    3, 4;
            cout << "(a > 0).all()   = " << (a > 0).all() << endl;
            cout << "(a > 0).any()   = " << (a > 0).any() << endl;
            cout << "(a > 0).count() = " << (a > 0).count() << endl;
            cout << endl;
            cout << "(a > 2).all()   = " << (a > 2).all() << endl;
            cout << "(a > 2).any()   = " << (a > 2).any() << endl;
            cout << "(a > 2).count() = " << (a > 2).count() << endl;
        }

        void Visitors() {
            LOG();
            // 这是在矩阵和数组的所有元素中
            //想要获得一个系数在Matrix或Array中的位置时，访问者很有用。
            //最简单的示例是maxCoeff（＆x，＆y）和minCoeff（＆x，＆y），可用于查找Matrix或Array中最大或最小系数的位置。
            //传递给访问者的参数是指向要存储行和列位置的变量的指针。这些变量应为Index类型，

            Eigen::MatrixXf m(2, 2);

            m << 1, 2,
                    3, 4;
            //get location of maximum
            MatrixXf::Index maxRow, maxCol;
            float max = m.maxCoeff(&maxRow, &maxCol);
            //get location of minimum
            MatrixXf::Index minRow, minCol;
            float min = m.minCoeff(&minRow, &minCol);
            cout << "Max: " << max << ", at: " << maxRow << "," << maxCol << endl;
            cout << "Min: " << min << ", at: " << minRow << "," << minCol << endl;
        }

        void PartialReductions() {
            LOG();
            // 这是在矩阵或数组的列向量和行向量中
            // 记住,element-wise是按元素的，那么colwise()或rowwise()表示按列或行的
            //部分归约是可以在Matrix或Array上按列或按行操作的归约，对每个列或行应用归约运算并返回具有相应值的列或行向量。
            //部分缩减适用于colwise（）或rowwise（）
            //一个简单的示例是获取给定矩阵中每一列中元素的最大值，并将结果存储在行向量中：
            // column-wise返回行向量，row-wise返回列向量。啥意思？应该设计底层操作，以后再看   ******TODO*******
            //注意： column-wise operations return a row vector, while row-wise operations return a column vector.
            Eigen::MatrixXf mat(2, 4);
            mat << 1, 2, 6, 9,
                    3, 1, 7, 2;

            std::cout << "Column's maximum: " << std::endl
                      << mat.colwise().maxCoeff() << std::endl; // 对于矩阵mat的每一列，取最大系数值

            // 也可以对行操作
            std::cout << "Row's maximum: " << std::endl
                      << mat.rowwise().maxCoeff() << std::endl; // 对于矩阵mat的每一行，取最大系数值
        }

        void CombiningPartialReductionsWithOtherOperations() {
            LOG();

            MatrixXf mat(2, 4);
            mat << 1, 2, 6, 9,
                    3, 1, 7, 2;

            MatrixXf::Index maxIndex;
            float maxNorm = mat.colwise().sum().maxCoeff(&maxIndex); //  对于矩阵的每一列中的元素求和，结果的最大系数在第2列

            std::cout << "Maximum sum at position " << maxIndex << std::endl;
            std::cout << "The corresponding vector is: " << std::endl;
            std::cout << mat.col(maxIndex) << std::endl;
            std::cout << "And its sum is is: " << maxNorm << std::endl;
            // Output is :
            //Maximum sum at position 2
            // The corresponding vector is:
            // 6
            // 7
            // And its sum is is: 13
        }

        void Broadcasting() {
            LOG();
            //广播背后的概念类似于部分归约，区别在于广播构造了一个表达式，其中向量（列或行）通过在一个方向上复制而被解释为矩阵。
            //一个简单的示例是将某个列向量添加到矩阵中的每一列。这可以通过以下方式完成：
            Eigen::MatrixXf mat(2, 4);
            Eigen::VectorXf v(2);
            mat << 1, 2, 6, 9,
                    3, 1, 7, 2;
            v << 0,
                    1;

            //add v to each column of m
            //mat.colwise() += v用两种等效的方式解释指令。
            //它将向量添加v到矩阵的每一列。或者，可以将其解释为重复向量v四次以形成四乘二矩阵，然后将其加到mat
            mat.colwise() += v;

            std::cout << "Broadcasting result: " << std::endl;
            std::cout << mat << std::endl;

            // Output is:
            // Broadcasting result:
            // 1 2 6 9
            // 4 2 8 3

            // 在矩阵上，我们可以执行-=,+=,+,-操作，但是不能进行*=,/=,*,/操作
            // 在数组上我们可执行*=,/=,*,/操作
            // If you want multiply column 0 of a matrix mat with v(0), column 1 with v(1), and so on, then use mat = mat * v.asDiagonal().
            // 要逐列或逐行添加的向量必须为Vector类型，并且不能为Matrix。
            // 如果不满足，则会出现编译时错误。
            //这也意味着，在使用Matrix进行操作时，广播操作只能应用于Vector类型的对象。这同样适用于数组类VectorXf是ArrayXf。与往常一样，您不应在同一表达式中混合使用数组和矩阵。

            // 同样，也可以对行执行此操作
            {
                Eigen::MatrixXf mat(2, 4);
                Eigen::VectorXf v(4);

                mat << 1, 2, 6, 9,
                        3, 1, 7, 2;

                v << 0, 1, 2, 3;

                //add v to each row of m
                mat.rowwise() += v.transpose();

                std::cout << "Broadcasting result: " << std::endl;
                std::cout << mat << std::endl;

                //  Broadcasting result:
                //  1  3  8 12
                //  3  2  9  5
            }
        }

        void CombiningBroadcastingWithOtherOperations() {
            LOG();
            // 广播还可以与其他操作（例如矩阵或阵列操作），归约和部分归约相结合。
            //现在已经介绍了广播，约简和部分约简，我们可以深入研究一个更高级的示例，该示例v在矩阵的列中找到向量的最近邻m。
            //欧几里德距离将在本示例中使用，计算具有部分归约名为squaredNorm()的平方欧几里德距离：
            Eigen::MatrixXf m(2, 4);
            Eigen::VectorXf v(2);

            m << 1, 23, 6, 9,
                    3, 11, 7, 2;

            v << 2,
                    3;
            MatrixXf::Index index;
            // find nearest neighbour
            (m.colwise() - v).colwise().squaredNorm().minCoeff(&index);
            cout << "Nearest neighbour is column " << index << ":" << endl;
            cout << m.col(index) << endl;
        }
    } // namespace Section6_ReductionsVisitorsBroadcasting
    //本页说明了如何使用“原始” C / C ++数组。这在各种情况下都可能有用，特别是在将其他库中的向量和矩阵“导入”Eigen。
    //有时您可能要在Eigen中使用预定义的数字数组(C++)作为矢量或矩阵(Eigen)。
    //一种选择是复制数据，但最常见的情况是您可能希望将此内存。幸运的是，使用Map类非常容易。
    // Map 类 实现C++中的数组内存和Eigen对象的交互
    // Map< Matrix<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>  >
    // 请注意，在这种默认情况下，Map仅需要一个模板参数。
    // 要构造Map变量，您还需要其他两条信息：指向定义系数数组的内存区域的指针，以及所需的矩阵或矢量形状。(注意区分模板参数和函数形参)
    // 例如，要定义一个float在编译时确定大小的矩阵，您可以执行以下操作：
    // Map <MatrixXf> mf（pf，rows，columns）;
    // 其中pf是一个float *指向的存储器阵列。固定大小的整数只读向量可能会声明为
    // Map <const Vector4i> mi（pi）;
    // 其中pi是int *。在这种情况下，不必将大小传递给构造函数，因为它已经由Matrix / Array类型指定。

    // 请注意，Map没有默认的构造函数。您必须传递一个指针来初始化对象。但是，您可以解决此要求（请参阅更改Map数组）。

    // Map足够灵活，可以容纳各种不同的数据表示形式。还有其他两个（可选）模板参数：
    // Map<typename MatrixType,    int MapOptions,     typename StrideType>
    //MapOptions指定指针是Aligned还是Unaligned。默认值为Unaligned。
    //StrideType允许您使用Stride类为内存阵列指定自定义布局。一个示例是指定数据数组以行优先格式进行组织MapConstruct()
    namespace Section7_MapClass {

        void MapConstruct() {
            LOG();
            int array[8];
            for (int i = 0; i < 8; ++i)
                array[i] = i;
            cout << "Column-major:\n"
                 << Map<Matrix<int, 2, 4>>(array) << endl;
            cout << "Row-major:\n"
                 << Map<Matrix<int, 2, 4, RowMajor>>(array) << endl;
            cout << "Row-major using stride:\n"
                 << Map<Matrix<int, 2, 4>, Unaligned, Stride<1, 4>>(array) << endl;
            //Output is:
            //Column-major:
            // 0 2 4 6
            // 1 3 5 7
            // Row-major:
            // 0 1 2 3
            // 4 5 6 7
            // Row-major using stride:
            // 0 1 2 3
            // 4 5 6 7

            //However, Stride is even more flexible than this; for details, see the documentation for the Map and Stride classes.
        }

        void UsingMapVariables() {
            LOG();
            typedef Matrix<float, 1, Dynamic> MatrixType;
            typedef Map<MatrixType> MapType;
            typedef Map<const MatrixType> MapTypeConst; // a read-only map
            const int n_dims = 5;

            MatrixType m1(n_dims), m2(n_dims);
            m1.setRandom();
            m2.setRandom();
            float *p = &m2(0);                     // get the address storing the data for m2
            MapType m2map(p, m2.size());           // m2map shares data with m2
            MapTypeConst m2mapconst(p, m2.size()); // a read-only accessor for m2
            cout << "m1: " << m1 << endl;
            cout << "m2: " << m2 << endl;
            cout << "Squared euclidean distance: " << (m1 - m2).squaredNorm() << endl;
            cout << "Squared euclidean distance, using map: " << (m1 - m2map).squaredNorm() << endl;
            m2map(3) = 7; // this will change m2, since they share the same array
            cout << "Updated m2: " << m2 << endl;
            cout << "m2 coefficient 2, constant accessor: " << m2mapconst(2) << endl;
            /* m2mapconst(2) = 5; */ // this yields a compile-time error

            //Output is:
            // m1:   0.68 -0.211  0.566  0.597  0.823
            // m2: -0.605  -0.33  0.536 -0.444  0.108
            // Squared euclidean distance: 3.26
            // Squared euclidean distance, using map: 3.26
            // Updated m2: -0.605  -0.33  0.536      7  0.108
            // m2 coefficient 2, constant accessor: 0.536
        }

        void ChangingTheMappedArray() {
            LOG();
            //可以使用C ++“ placement new"(位置new，在程序员给定的内存放置元素) 语法更改已声明的Map对象的数组
            ////尽管有出现，但它不会调用内存分配器，因为语法指定了存储结果的位置。(具体参考《C++primer》中的内容)
            // 简单的说，位置new只是在指定位置写入内容，并不会像new一样，先在堆上分配内存，然后再依次初始化对象，
            // 这也是为什么叫位置new，因为它会按照我们指定的位置构造对象
            int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            Map<RowVectorXi> v(data, 4);
            cout << "The mapped vector v is: " << v << "\n";
            new(&v) Map<RowVectorXi>(data + 4, 5);
            cout << "Now v is: " << v << "\n";
            //The mapped vector v is: 1 2 3 4
            //Now v is: 5 6 7 8 9

            // TODO : 教程中没有指定n_matrices
            // Map<Matrix3f> A(NULL); // don't try to use this matrix yet!
            // VectorXf b(n_matrices);
            // for (int i = 0; i < n_matrices; i++)
            // {
            //         new (&A) Map<Matrix3f>(get_matrix_pointer(i));
            //         b(i) = A.trace();
            // }
        }
    } // namespace Section7_MapClass
    //Eigen还没有提供方便的方法来切片或重塑矩阵。但是，可以使用Map类轻松模拟这些功能。
    namespace Section8_ReshapeSlicing {
        void Reshape() {
            LOG();
            // 重塑
            //整形操作在于修改矩阵的大小，同时保持相同的系数。
            //除了修改输入矩阵本身（这对于编译时大小而言是不可能的）之外，
            //该方法还包括使用Map类在存储上创建不同的视图。这是创建矩阵的一维线性视图的典型示例：
            MatrixXf M1(3, 3); // Column-major storage
            // 注意：逗号初始化是为了方便我们输入矩阵，但是底层存储是按照列主的顺序存储的
            M1 << 1, 2, 3,
                    4, 5, 6,
                    7, 8, 9;
            Map<RowVectorXf> v1(M1.data(), M1.size());
            cout << "v1:" << endl
                 << v1 << endl;
            Matrix<float, Dynamic, Dynamic, RowMajor> M2(M1);
            Map<RowVectorXf> v2(M2.data(), M2.size());
            cout << "v2:" << endl
                 << v2 << endl;
            //Output is:
            // v1:
            // 1 4 7 2 5 8 3 6 9
            // v2:
            // 1 2 3 4 5 6 7 8 9

            //注意输入矩阵的存储顺序如何修改线性视图中系数的顺序。这是另一个将2x6矩阵重塑为6x2矩阵的示例：
            {
                MatrixXf M1(2, 6); // Column-major storage
                M1 << 1, 2, 3, 4, 5, 6,
                        7, 8, 9, 10, 11, 12;
                Map<MatrixXf> M2(M1.data(), 6, 2);
                cout << "M2:" << endl
                     << M2 << endl;
                //Output is:
                //  M2:
                //  1  4
                //  7 10
                //  2  5
                //  8 11
                //  3  6
                //  9 12
            }
        }

        void Slicing() {
            LOG();
            //切片包括获取一组在矩阵内均匀间隔的行，列或元素。再次，Map类可以轻松模仿此功能。
            //例如，可以跳过向量中的每个P元素：
            RowVectorXf v = RowVectorXf::LinSpaced(20, 0, 19);
            cout << "Input:" << endl
                 << v << endl;
            Map<RowVectorXf, 0, InnerStride<2>> v2(v.data(), v.size() / 2);
            cout << "Even:" << v2 << endl;
            //Output is:
            //  Input:
            //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
            // Even: 0  2  4  6  8 10 12 14 16 18

            //一个人可以根据实际的存储顺序，使用适当的外步幅或内步幅来占据三列中的一列：
            // TODO:这个好像有点复杂，让我缓缓～～～～
            MatrixXf M1 = MatrixXf::Random(3, 8);
            cout << "Column major input:" << endl
                 << M1 << "\n";
            Map<MatrixXf, 0, OuterStride<>> M2(M1.data(), M1.rows(), (M1.cols() + 2) / 3,
                                               OuterStride<>(M1.outerStride() * 3));
            cout << "1 column over 3:" << endl
                 << M2 << "\n";
            typedef Matrix<float, Dynamic, Dynamic, RowMajor> RowMajorMatrixXf;
            RowMajorMatrixXf M3(M1);
            cout << "Row major input:" << endl
                 << M3 << "\n";
            Map<RowMajorMatrixXf, 0, Stride<Dynamic, 3>> M4(M3.data(), M3.rows(), (M3.cols() + 2) / 3,
                                                            Stride<Dynamic, 3>(M3.outerStride(), 3));
            cout << "1 column over 3:" << endl
                 << M4 << "\n";
            //Column major input:
            //    0.68   0.597   -0.33   0.108   -0.27   0.832  -0.717  -0.514
            //  -0.211   0.823   0.536 -0.0452  0.0268   0.271   0.214  -0.726
            //   0.566  -0.605  -0.444   0.258   0.904   0.435  -0.967   0.608
            // 1 column over 3:
            //    0.68   0.108  -0.717
            //  -0.211 -0.0452   0.214
            //   0.566   0.258  -0.967
            // Row major input:
            //    0.68   0.597   -0.33   0.108   -0.27   0.832  -0.717  -0.514
            //  -0.211   0.823   0.536 -0.0452  0.0268   0.271   0.214  -0.726
            //   0.566  -0.605  -0.444   0.258   0.904   0.435  -0.967   0.608
            // 1 column over 3:
            //    0.68   0.108  -0.717
            //  -0.211 -0.0452   0.214
            //   0.566   0.258  -0.967
        }
    } // namespace Part8_ReshapeSlicing
    namespace Section9_Aliasing {
        // 这一节比较绕
        // 在Eigen中，混淆(aliasing)是指相同的矩阵（或数组或向量）出现在赋值运算符的左侧和右侧的赋值语句;
        // 例如，A = AB ,  a = a^Tb  A=A*A
        // 产生混淆的原因是Eigen采用惰性求值
        // 混淆可能是有害的，也可能是无害的,有害的混淆，导致可能不正确的结果，无害的混淆可以产生正确的结果

        // 有害混淆
        // 使用.eval()方法，可以解决混淆问题。具体的说，.eval()方法生成临时对象，然后再执行赋值操作
        //  如果Eigen中假定该操作是混淆的，那么它会自动使用.eval()方法，而不需要我们显示调用

        // 无害混淆是我们不需要评估，它对系数计算无害。这包括标量乘法和矩阵或数组加法。

        // 将两个矩阵相乘时，Eigen会假定发生混叠(注意，Eigen3.3以后的版本中需要区分目标矩阵大小是否改变了)
        //如果您知道没有混淆，则可以使用noalias()。

        // 在所有其他情况下，Eigen假定不存在混叠问题，因此如果实际上发生混叠，则会给出错误的结果。
        // 为防止这种情况，您必须使用eval（）或xxxInPlace（）函数之一。

        void Aliasing() {
            LOG();
            //在Eigen中，混淆(aliasing)是指相同的矩阵（或数组或向量）出现在赋值运算符的左侧和右侧的赋值语句。
            // 例如mat = 2 * mat(虽混淆但无害);或mat = mat.transpose();(有害的混淆)。

            MatrixXi mat(3, 3);
            mat << 1, 2, 3, 4, 5, 6, 7, 8, 9;
            cout << "Here is the matrix mat:\n"
                 << mat << endl;
            // This assignment shows the aliasing problem
            mat.bottomRightCorner(2, 2) = mat.topLeftCorner(2, 2);
            cout << "After the assignment, mat = \n"
                 << mat << endl;

            // Output  is:
            // Here is the matrix mat:
            // 1 2 3
            // 4 5 6
            // 7 8 9
            // After the assignment, mat =
            // 1 2 3
            // 4 1 2
            // 7 4 1

            // 理解混淆的关键是了解惰性求值

            // 输出结果不是人们所期望的，问题是产生了有害混淆
            // mat.bottomRightCorner（2,2）= mat.topLeftCorner（2,2）;
            // 该赋值具有混淆aliasing）：系数mat(1,1)既出现在mat.bottomRightCorner(2,2)分配左侧的块中mat.topLeftCorner(2,2)，又出现在右侧的块中。
            // 分配后，右下角的（2,2）项应具有mat(1,1)分配前的值5。但是，输出显示mat(2,2)实际上为1。
            // 问题在于Eigen使用了惰性求值。结果类似于
            // mat（1,1）= mat（0,0）;
            // mat（1,2）= mat（0,1）;
            // mat（2,1）= mat（1,0）;
            // mat（2,2）= mat（1,1）;
            // 因此，mat(2,2)分配了新值mat(1,1)而不是旧值。
            // 可以通过调用eval()解决此问题(注:eval()负责生成临时对象而避免混淆)
            // 尝试缩小矩阵时，混淆也会发生。
            // 例如，表达式vec = vec.head(n)和mat = mat.block(i,j,r,c)具有混淆。

            //通常，在编译时无法检测到混淆：如果mat在第一个示例中稍大一点，则块将不会重叠，也不会出现混淆问题。
            //但是Eigen确实会在运行时检测到一些混淆实例。Matrix和向量算术中提到了以下显示别名的示例：

            // 注意：这段代码会报错～～～～～～

            // Matrix2i a;
            // a << 1, 2, 3, 4;
            // cout << "Here is the matrix a:\n"
            //      << a << endl;
            // a = a.transpose(); // !!! do NOT do this !!!
            // cout << "and the result of the aliasing effect:\n"
            //      << a << endl;

            // 输出显示混淆(alising)问题。
            // 但是，默认情况下，Eigen使用运行时断言来检测到此情况并退出，并显示如下消息
            // void Eigen::DenseBase<Derived>::checkTransposeAliasing(const OtherDerived&) const
            // [with OtherDerived = Eigen::Transpose<Eigen::Matrix<int, 2, 2, 0, 2, 2> >, Derived = Eigen::Matrix<int, 2, 2, 0, 2, 2>]:
            // Assertion `(!internal::check_transpose_aliasing_selector<Scalar,internal::blas_traits<Derived>::IsTransposed,OtherDerived>::run(internal::extract_data(derived()), other))
            // && "aliasing detected during transposition, use transposeInPlace() or evaluate the rhs into a temporary using .eval()"' failed.

            //用户可以通过定义EIGEN_NO_DEBUG宏来关闭Eigen的运行时断言
        }

        void ResolvingAliasingIssues() {
            LOG();
            //解决方法：Eigen必须将右侧完全看作一个临时矩阵/数组，然后将其分配给左侧。
            //函数**eval()**正是这样做的,作用为生成一个临时对象
            MatrixXi mat(3, 3);
            mat << 1, 2, 3, 4, 5, 6, 7, 8, 9;
            cout << "Here is the matrix mat:\n"
                 << mat << endl;
            // The eval() solves the aliasing problem
            mat.bottomRightCorner(2, 2) = mat.topLeftCorner(2, 2).eval();
            cout << "After the assignment, mat = \n"
                 << mat << endl;

            //相同的解决方案也适用于第二示例中，与转置：只需更换线a = a.transpose();用a = a.transpose().eval();。但是，在这种常见情况下，有更好的解决方案。
            //Eigen提供了专用函数transposeInPlace()，该函数通过其转置来替换矩阵。如下所示：
            MatrixXf a(2, 3);
            a << 1, 2, 3, 4, 5, 6;
            cout << "Here is the initial matrix a:\n"
                 << a << endl;
            a.transposeInPlace();
            cout << "and after being transposed:\n"
                 << a << endl;

            //如果xxxInPlace()函数可用，则最好使用它，因为它可以更清楚地指示您正在做什么。
            //这也可以让Eigen更积极地进行优化。这些是提供的一些xxxInPlace()函数：
            // Original function	                      In-place function
            // MatrixBase::adjoint()	          MatrixBase::adjointInPlace()
            // DenseBase::reverse()	                 DenseBase::reverseInPlace()
            // LDLT::solve()	                            LDLT::solveInPlace()
            // LLT::solve()	                                      LLT::solveInPlace()
            // TriangularView::solve()	        TriangularView::solveInPlace()
            // DenseBase::transpose()	       DenseBase::transposeInPlace()

            //在特殊情况下，矩阵或向量使用类似的表达式缩小vec = vec.head(n)，则可以使用conservativeResize()。
        }

        void AliasingAndComponentWiseOperations() {
            LOG();
            //如果同一矩阵或数组同时出现在赋值运算符的左侧和右侧，则可能很危险，因此通常有必要显示地评估右侧
            //但是，应用基于元素的操作（例如矩阵加法，标量乘法和数组乘法）是安全的。
            //以下示例仅具有基于组件的操作。因此，即使相同的矩阵出现在赋值符号的两侧，也不需要eval()。
            MatrixXf mat(2, 2);
            mat << 1, 2, 4, 7;
            cout << "Here is the matrix mat:\n"
                 << mat << endl
                 << endl;
            mat = 2 * mat;
            cout << "After 'mat = 2 * mat', mat = \n"
                 << mat << endl
                 << endl;
            mat = mat - MatrixXf::Identity(2, 2);
            cout << "After the subtraction, it becomes\n"
                 << mat << endl
                 << endl;
            ArrayXXf arr = mat;
            arr = arr.square();
            cout << "After squaring, it becomes\n"
                 << arr << endl
                 << endl;
            // Combining all operations in one statement:
            mat << 1, 2, 4, 7;
            mat = (2 * mat - MatrixXf::Identity(2, 2)).array().square();
            cout << "Doing everything at once yields\n"
                 << mat << endl
                 << endl;
            // Output is:
            // Here is the matrix mat:
            // 1 2
            // 4 7

            // After 'mat = 2 * mat', mat =
            //  2  4
            //  8 14

            // After the subtraction, it becomes
            //  1  4
            //  8 13

            // After squaring, it becomes
            //   1  16
            //  64 169

            // Doing everything at once yields
            //   1  16
            //  64 169
            //通常，如果表达式右侧的（i，j）条目仅取决于左侧矩阵或数组的（i，j）条目
            //而不依赖于其他任何表达式，则赋值是安全的。在这种情况下，不必显示地评估右侧(.evl())。
        }

        void AliasingAndMatrixMultiplication() {
            LOG();

            //在目标矩阵**未调整大小**的情况下，矩阵乘法是Eigen中唯一假定默认情况下为混淆的。
            // 若假定混淆，则会使用eval()生成临时对象,所以是安全的。
            //因此，如果matA是平方矩阵，则该语句matA = matA * matA是安全的。
            //Eigen中的所有其他操作都假定没有混淆问题，这是因为结果被分配给了不同的矩阵，或者因为它是逐个元素的操作。
            {
                MatrixXf matA(2, 2);
                matA << 2, 0, 0, 2;
                matA = matA * matA;
                cout << matA << endl;
            }

            // 但是，这是有代价的。执行表达式时matA = matA * matA
            // Eigen会在计算后的临时矩阵中评估赋值给matA的乘积。
            //虽然可以，但是在将乘积分配给其他矩阵（例如matB = matA * matA）时，Eigen会执行相同的操作。
            //在这种情况下，直接评估matB,
            // 而不是先将matA*matA生成临时对象，然后评估临时对象，最后将临时对象赋值给矩阵matB更高效
            //我们可以使用noalias函数指示没有混淆，如下所示：matB.noalias() = matA * matA。
            //这使Eigen可以matA * matA直接将矩阵乘积在matB中评估。
            {
                MatrixXf matA(2, 2), matB(2, 2);
                matA << 2, 0, 0, 2;
                // Simple but not quite as efficient
                matB = matA * matA;
                cout << matB << endl;
                // More complicated but also more efficient
                matB.noalias() = matA * matA;
                cout << matB << endl;
                // Output is:
                //4 0
                // 0 4

                // 4 0
                // 0 4
            }

            {
                //当然，不应该在实际上发生混淆时使用noalias()，如果这样做，则**可能**会得到错误的结果：
                //报错吗？我的平台上没报错
                MatrixXf matA(2, 2);
                matA << 2, 0, 0, 2;
                matA.noalias() = matA * matA;
                cout << matA << endl;
                // Output is:
                //4 0
                // 0 4
            }

            {
                //此外，从Eigen 3.3 开始，
                //如果调整了目标矩阵的大小(注意，上文中的操作假定目标矩阵大小不变)
                //并且未将乘积直接分配给目标，则不假定混淆。因此，以下示例也是错误的：
                MatrixXf A(2, 2), B(3, 2);
                B << 2, 0, 0, 3, 1, 1;
                A << 2, 0, 0, -2;
                A = (B * A).cwiseAbs(); // 由于不假定混淆，所以需要我们显示评价
                cout << A << endl;
                //报错吗？我的平台上没报错
                // Output is
                // 4 0
                // 0 6
                // 2 2
            }

            {
                //对于任何混淆问题，您都可以通过在赋值之前显式评估表达式来解决它：
                MatrixXf A(2, 2), B(3, 2);
                B << 2, 0, 0, 3, 1, 1;
                A << 2, 0, 0, -2;
                A = (B * A).eval().cwiseAbs();
                cout << A << endl;

                // Output is
                // 4 0
                // 0 6
                // 2 2
            }
        }

    } // namespace Section9_Aliasing
    //矩阵和二维数组有两种不同的存储顺序：列优先和行优先。本页说明了这些存储顺序以及如何指定应使用的存储顺序。
    //矩阵的条目形成一个二维网格。但是，当矩阵存储在存储器中时，必须以某种方式线性排列条目。有两种主要方法可以做到这一点，按行和按列。
    //我们说矩阵是**按行优先存储**。首先存储整个第一行，然后存储整个第二行，依此类推。
    //另一方面，如果矩阵是逐列存储的，则以主列顺序存储，从整个第一列开始，然后是整个第二列，依此类推

    //可以通过Options为Matrix或Array指定模板参数来设置矩阵或二维数组的存储顺序。
    //由于Matrix类解释，Matrix类模板有六个模板参数，
    // 其中三个是强制性的（Scalar，RowsAtCompileTime和ColsAtCompileTime）
    // 三个是可选的（Options，MaxRowsAtCompileTime和MaxColsAtCompileTime）。
    //如果Options参数设置为RowMajor，则矩阵或数组以行优先顺序存储；
    //如果将其设置为ColMajor，则以列优先顺序存储。
    //如果未指定存储顺序，则Eigen默认将条目存储在column-major中。
    //如果方便的typedef（Matrix3f，ArrayXXd等）也是默认按列存储。

    //可以将使用一种存储顺序的矩阵和数组分配给使用另一种存储顺序的矩阵和数组，如一个按行存储的矩阵使用按列存储矩阵初始化。
    //Eigen将自动对元素重新排序。更一般而言，按行存储矩阵和按列存储矩阵可以根据需要在表达式中混合使用。

    //选择哪个存储顺序？
    //那么，您应该在程序中使用哪个存储顺序？这个问题没有简单的答案。这取决于您的应用程序。请记住以下几点：

    //您的用户可能希望您使用特定的存储顺序。或者，您可以使用Eigen以外的其他库，并且这些其他库可能需要一定的存储顺序。
    //在这些情况下，在整个程序中使用此存储顺序可能是最简单，最快的。
    //当矩阵以行优先顺序存储时，逐行遍历矩阵的算法会更快，因为数据位置更好。同样，对于主要列矩阵，逐列遍历更快。可能需要尝试一下以找出对您的特定应用程序更快的方法。
    //Eigen中的默认值是column-major。自然，对Eigen库的大多数开发和测试都是使用列主矩阵完成的。
    //这意味着，即使我们旨在透明地支持列主存储和行主存储顺序，Eigen库也最好与列主存储矩阵配合使用。
    namespace Section10_StorageOrders {

        void testColumnAndRowMajorStorage() {
            LOG();
            // PlainObjectBase::data()  返回第一个元素的内存位置，和C++的数组名作用一样

            Matrix<int, 3, 4, ColMajor> Acolmajor;
            Acolmajor << 8, 2, 2, 9,
                    9, 1, 4, 4,
                    3, 5, 4, 5;
            cout << "The matrix A:" << endl;
            cout << Acolmajor << endl
                 << endl;
            cout << "In memory (column-major):" << endl;
            for (int i = 0; i < Acolmajor.size(); i++)
                cout << *(Acolmajor.data() + i) << "  ";
            cout << endl
                 << endl;
            Matrix<int, 3, 4, RowMajor> Arowmajor = Acolmajor;
            cout << "In memory (row-major):" << endl;
            for (int i = 0; i < Arowmajor.size(); i++)
                cout << *(Arowmajor.data() + i) << "  ";
            cout << endl;

            // Output is
            // The matrix A:
            // 8 2 2 9
            // 9 1 4 4
            // 3 5 4 5

            // In memory (column-major):
            // 8  9  3  2  1  5  2  4  4  9  4  5

            // In memory (row-major):
            // 8  2  2  9  9  1  4  4  3  5  4  5
        }

    } // namespace Section10_StorageOrders
    namespace Section11_AlignmentIssues {
        // 这节比较抽象，知道咋用就好！
        // TODO 继续翻译！

        // 对齐错误
        //Eigen::internal::matrix_array<T, Size, MatrixOptions, Align>::internal::matrix_array()
        // [with T = double, int Size = 2, int MatrixOptions = 2, bool Align = true]:
        // Assertion `(reinterpret_cast<size_t>(array) & (sizemask)) == 0 && "this assertion
        // is explained here: http://eigen.tuxfamily.org/dox-devel/group__TopicUnalignedArrayAssert.html
        //      READ THIS WEB PAGE !!! ****"' failed.

        // 首先找到程序触发位置，
        // 例如，
        //$gdb ./my_program
        //>run
        //...
        //>bt
        //然后与下面的4种原因对号入座，修改代码

        // 二 四种原因

        // 原因1:结构体中具有Eigen对象成员
        //请注意，此处Eigen :: Vector2d仅用作示例，
        //更一般而言，所有固定大小的可矢量化Eigen类型都会出现此问题
        //固定大小的可矢量化Eigen类型是如果它具有固定的大小并且大小是16字节的倍数
        // Eigen::Vector2d
        // Eigen::Vector4d
        // Eigen::Vector4f
        // Eigen::Matrix2d
        // Eigen::Matrix2f
        // Eigen::Matrix4d
        // Eigen::Matrix4f
        // Eigen::Affine3d
        // Eigen::Affine3f
        // Eigen::Quaterniond
        // Eigen::Quaternionf

        // 首先, "固定大小"应该清楚：如果在编译时，Eigen对象的行数和列数是固定的，则其固定大小。
        // 因此，例如Matrix3f具有固定大小，但MatrixXf没有（固定大小的对立是动态大小）。
        // 固定大小的Eigen对象的系数数组是普通的“静态数组”，不会动态分配。例如，Matrix4f后面的数据只是一个“float array[16]”。
        // 固定大小的对象通常很小，这意味着我们要以零的运行时开销（在内存使用和速度方面）来处理它们。
        // 现在，矢量化（SSE和AltiVec）都可以处理128位数据包。此外，出于性能原因，这些数据包必须具有128位对齐。
        //因此，事实证明，固定大小的Eigen对象可以向量化的唯一方法是，
        //如果它们的大小是128位或16个字节的倍数。然后，Eigen将为这些对象请求16字节对齐，并且此后将依赖于这些对象进行对齐，因此不会执行运行时检查以进行对齐。
        // class Foo
        // {
        //   //...
        //   Eigen::Vector2d v;
        //   //...
        // };
        // //...
        // Foo *foo = new Foo;

        // Eigen需要Eigen :: Vector2d的数组（2个双精度）的128位对齐。对于GCC，这是通过属性（（aligned（16）））完成的。
        // Eigen重载了Eigen :: Vector2d的“ operator new”，因此它将始终返回128位对齐的指针。
        // 因此，通常情况下，您不必担心任何事情，Eigen会为您处理对齐...

        // ...除了一种情况。当您具有上述的Foo类，并且如上所述动态分配新的Foo时，则由于Foo没有对齐“ operator new”，因此返回的指针foo不一定是128位对齐的。
        // 然后，成员v的alignment属性相对于类的开头foo。如果foo指针未对齐，则foo-> v也不会对齐！
        // 解决方案是让Foo类具有一致的“Operator new”

        // 解决方法：
        //如果定义的结构具有固定大小的可矢量化Eigen类型的成员，
        //则必须重载其“ operator new”，以便它生成16字节对齐的指针。
        //幸运的是，Eigen为您提供了一个宏EIGEN_MAKE_ALIGNED_OPERATOR_NEW来为您执行此操作。
        //换句话说：您有一个类，该类具有固定大小的可矢量化Eigen对象作为成员，然后动态创建该类的对象。

        //很简单，您只需将EIGEN_MAKE_ALIGNED_OPERATOR_NEW宏放在您的类的public部分
        // class Foo
        // {
        //         Eigen::Vector2d v;
        //         public : EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        // };
        // Foo *foo = new Foo;
        //该宏使“ new Foo”始终返回对齐的指针。
        //一个 Eigen::Vector2d有两个double类型，一个double为8字节=64位，则一个Eigen::Vector2d为128位
        //这恰好是SSE数据包的大小，这使得可以使用SSE对该向量执行各种操作。
        //但是SSE指令（至少Eigen使用的是快速指令）需要128位对齐。否则会出现段错误。
        // 出于这个原因，Eigen自己通过执行以下两项工作自行要求对Eigen :: Vector2d进行128位对齐：


        //原因2：STL容器或手动内存分配
        ///如果您将Stl容器（例如std :: vector，std :: map等）
        //与Eigen对象或包含Eigen对象的类一起使用，
        // std::vector<Eigen::Matrix2f> my_vector;
        // struct my_class { ... Eigen::Matrix2f m; ... };
        // std::map<int, my_class> my_map;
        //请注意，此处Eigen :: Matrix2f仅用作示例，更一般而言，对于所有固定大小的可矢量化Eigen类型和具有此类Eigen对象作为member的结构，都会出现此问题。
        //任何类/函数都会绕过new分配内存的运算符，也就是通过执行自定义内存分配，然后调用placement new运算符，
        // 也会出现相同的问题。例如，通常就是这种情况，std::make_shared或者std::allocate_shared解决方案是使用对齐的分配器，
        //如STL容器解决方案中所述。

        //原因3：通过值传递Eigen对象
        //如果您代码中的某些函数正在通过值传递Eigen对象，例如这样，
        //void func(Eigen::Vector4d v);
        //那么您需要阅读以下单独的页面：将Eigen对象按值传递给函数。
        //请注意，此处Eigen :: Vector4d仅用作示例，更一般而言，所有固定大小的可矢量化Eigen类型都会出现此问题

        //原因4:编译器对堆栈对齐做出错误假设（例如Windows上的GCC）
        //这是在Windows上使用GCC（例如MinGW或TDM-GCC）的人们的必读内容。
        //如果在声明这样的局部变量的无辜函数中有此断言失败，请执行以下操作：
        // void foo()
        // {
        //   Eigen::Quaternionf q;
        //   //...
        // }
        //那么您需要阅读以下单独的页面：编译器对堆栈对齐做出了错误的假设。
        //请注意，此处Eigen :: Quaternionf仅用作示例，更一般而言，所有固定大小的可矢量化Eigen类型都会出现此问题。

        // 3  该断言的一般解释:
        // 固定大小的矢量化Eigen对象必须绝对在16字节对齐的位置创建，否则寻址它们的SIMD指令将崩溃。
        // Eigen通常会为您解决这些对齐问题，方法是在固定大小的可矢量化对象上设置对齐属性，
        // 并重载他们的“ operator new”。
        // 但是，在某些极端情况下，这些对齐设置会被覆盖：它们是此断言的可能原因。

        // 4   我不在乎最佳矢量化，如何摆脱这些东西？
        // 三种可能性：
        // 使用Matrix，Array，Quaternion等对象的DontAlign选项会给您带来麻烦。
        // 这样，Eigen不会尝试对齐它们，因此不会采取任何特殊对齐方式。
        // 不利的一面是，您将为它们支付未对齐的加载/存储的成本，但是在现代CPU上，开销为null或边际的。
        // 定义EIGEN_DONT_ALIGN_STATICALLY。这将禁用所有16​​字节（或以上）的静态对齐代码，
        // 同时保持16字节（或以上）的堆对齐。这具有通过未对齐的存储区（由EIGEN_UNALIGNED_VECTORIZE控制）对固定大小的对象（如Matrix4d）进行矢量化的效果，同时保持动态大小的对象（如MatrixXd）的矢量化不变。
        //但是请注意，这会破坏ABI与静态对齐方式默认行为的兼容性。
        // 或同时定义EIGEN_DONT_VECTORIZE和EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT。
        // 这样可以保留16字节的对齐代码，从而保留ABI兼容性，但完全禁用向量化。
        // 如果您想知道为什么定义EIGEN_DONT_VECTORIZE本身并不能禁用16字节对齐和断言，则说明如下：
        // 它不会禁用断言，因为如果不执行矢量化，则正常运行的代码将在启用矢量化时突然崩溃。
        // 它不会禁用16字节对齐，因为这将意味着矢量化和非矢量化的代码不相互兼容ABI。
        // 即使对于仅开发内部应用程序的人，这种ABI兼容性也非常重要，
        // 例如，可能希望在同一应用程序中同时具有矢量化路径和非矢量化路径。

    } // namespace Section11_AlignmentIssues
} // namespace Chapter1_DenseMatrixAndArrary
void PrintMsg(int i) {
    cout << endl
         << "*******  Results   of   Chapter1   Section" << i << "  ********" << endl;
}

void TestChapter1Section1() {
    PrintMsg(1);
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::CoefficientAccessors();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::AssignmentAndResizing();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::CommaInitialization();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::ConvenienceTypedefs();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::FixedSizeAndDynamicSize();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::MatrixAndVectorCompileTime();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::MatrixAndVectorRunTime();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::OptionalTemplateParameters();
    Chapter1_DenseMatrixAndArrary::Section1_MatrixClass::Resizing();
}

void TestChapter1Section2() {
    PrintMsg(2);
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::AdditionAndSubtraction();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::ScalarMultiplicationAndDivision();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::ANoteAboutExpressionTemplates();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::TranspositionAndConjugation();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::MatrixMatrixAndMatrixVectorMultiplication();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::DotProductAndCrossProduct();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::BasicArithmeticReductionOperations();
    Chapter1_DenseMatrixAndArrary::Section2_MatrixAndVectorArithmetic::ValidityOfoperations();
}

void TestChapter1Section3() {
    PrintMsg(3);
    Chapter1_DenseMatrixAndArrary::Section3_ArrayAndCoefficientwiseOperations::AccessingValuesInsideAnArray();
    Chapter1_DenseMatrixAndArrary::Section3_ArrayAndCoefficientwiseOperations::AdditionAndSubtraction();
    Chapter1_DenseMatrixAndArrary::Section3_ArrayAndCoefficientwiseOperations::ArrayMultiplication();
    Chapter1_DenseMatrixAndArrary::Section3_ArrayAndCoefficientwiseOperations::ConvertingBetweenArrayAndMatrixExpressions();
    Chapter1_DenseMatrixAndArrary::Section3_ArrayAndCoefficientwiseOperations::OtherCoefficientwiseOperations();
}

void TestChapter1Section4() {
    PrintMsg(4);
    Chapter1_DenseMatrixAndArrary::Section4_BlockOperations::UsingBlockOperations();
    Chapter1_DenseMatrixAndArrary::Section4_BlockOperations::BlockAsLeftValue();
    Chapter1_DenseMatrixAndArrary::Section4_BlockOperations::ColumnsAndRows();
    Chapter1_DenseMatrixAndArrary::Section4_BlockOperations::CornerRelatedOperations();
    Chapter1_DenseMatrixAndArrary::Section4_BlockOperations::BlockOperationsForVectors();
}

void TestChapter1Section5() {
    PrintMsg(5);
    Chapter1_DenseMatrixAndArrary::Section5_AdvancedInitialization::CommaInitializer();
    Chapter1_DenseMatrixAndArrary::Section5_AdvancedInitialization::SpecialMatricesAndArrays();
    Chapter1_DenseMatrixAndArrary::Section5_AdvancedInitialization::UsageAsTemporaryObjects();
}

void TestChapter1Section6() {
    PrintMsg(6);
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::BooleanReductions();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::Broadcasting();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::CombiningBroadcastingWithOtherOperations();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::CombiningPartialReductionsWithOtherOperations();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::NormComputations();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::PartialReductions();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::Reductions();
    Chapter1_DenseMatrixAndArrary::Section6_ReductionsVisitorsBroadcasting::Visitors();
}

void TestChapter1Section7() {
    PrintMsg(7);
    Chapter1_DenseMatrixAndArrary::Section7_MapClass::ChangingTheMappedArray();
    Chapter1_DenseMatrixAndArrary::Section7_MapClass::MapConstruct();
    Chapter1_DenseMatrixAndArrary::Section7_MapClass::UsingMapVariables();
}

void TestChapter1Section8() {
    PrintMsg(8);

    Chapter1_DenseMatrixAndArrary::Section8_ReshapeSlicing::Reshape();
    Chapter1_DenseMatrixAndArrary::Section8_ReshapeSlicing::Slicing();
}

void TestChapter1Section9() {
    PrintMsg(9);

    Chapter1_DenseMatrixAndArrary::Section9_Aliasing::Aliasing();
    Chapter1_DenseMatrixAndArrary::Section9_Aliasing::ResolvingAliasingIssues();
    Chapter1_DenseMatrixAndArrary::Section9_Aliasing::AliasingAndComponentWiseOperations();
    Chapter1_DenseMatrixAndArrary::Section9_Aliasing::AliasingAndMatrixMultiplication();
}

void TestChapter1Section10() {
    PrintMsg(10);

    Chapter1_DenseMatrixAndArrary::Section10_StorageOrders::testColumnAndRowMajorStorage();
}

void TestChapter1Section11() {
    //当前Chapter11没有程序
}

int main() {
    // build pass
    TestChapter1Section1();

    // build pass
    TestChapter1Section2();

    // build pass
    TestChapter1Section3();

    // build pass
    TestChapter1Section4();

    // build pass
    TestChapter1Section5();

    // build pass
    TestChapter1Section6();

    // build pass
    TestChapter1Section7();

    // build pass
    TestChapter1Section8();

    // build pass
    TestChapter1Section9();

    // build pass
    TestChapter1Section10();

    //TODO:add the program of TestChapter1Section11()
    return 0;
}