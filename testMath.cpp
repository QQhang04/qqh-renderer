#include "global/base.h"
#include "math/math.h"
#include "math/matrix.h"

using namespace math;

int main() {
    using namespace math;

    // ===== 向量测试 =====
    vec2f v2_1(2.0f, 3.0f);
    vec2f v2_2 = v2_1;

    auto t2 = normalize(v2_1);
    t2.print();

    auto v2_add = v2_1 + v2_2;
    std::cout << "v2 add is:" << std::endl;
    v2_add.print();

    auto v2_multi = v2_1 * v2_2;
    std::cout << "v2 multi is:" << std::endl;
    v2_multi.print();

    auto v2_div = v2_2 / 2;
    std::cout << "v2 div is:" << std::endl;
    v2_div.print();

    vec4f v4(5.0f, 1.0f, 3.0f, 1.0f);
    //auto v4n = normalize(v4);
    //v4n.print();

    // ===== 矩阵测试 =====
    std::cout << "\n===== Matrix44 Test =====" << std::endl;
    Mat4f mA;
    mA.set(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        0, 0, 0, 1);
    std::cout << "Original Matrix:" << std::endl;
    mA.printMatrix();

    auto mATrans = math::transpose(mA);
    std::cout << "Transpose:" << std::endl;
    mATrans.printMatrix();

    auto mB = mA * 2.0f; // 标量乘
    std::cout << "Scalar *2:" << std::endl;
    mB.printMatrix();

    Mat4f identity = mA * math::inverse(mA); // 应当得到单位矩阵
    std::cout << "A * inverse(A):" << std::endl;
    identity.printMatrix();

    // 乘以转置再求逆，结果应为 A
    auto recover = math::inverse(mATrans) * mATrans;
    std::cout << "inverse(Transpose) * Transpose:" << std::endl;
    recover.printMatrix();

    mA.set(
        2, 0, 0, 0,
        0, 4, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 4);
    std::cout << "inverse:" << std::endl;
    mA = math::inverse(mA);
    mA.printMatrix();

    return 0;
}