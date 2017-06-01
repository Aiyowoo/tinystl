#ifndef TINYSTL_TYPETRAINTS_H
#define TINYSTL_TYPETRAINTS_H

namespace tinystl {
    struct TrueType {
    };

    struct FalseType {
    };

    template<typename T>
    struct TypeTraits {
        // 基本模板,所有都为 false
        using hasTrivialDefaultConstructor = FalseType;
        using hasTrivialCopyConstructor = FalseType;
        using hasTrivialAssignmentOperator = FalseType;
        using hasTrivialDestructor = FalseType;
        // 是否是与C兼容的数据,能被malloc产生,能使用memcpy复制
        using isPODType = FalseType;
    };

    template<>
    struct TypeTraits<bool> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<char> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<signed char> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<unsigned char> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<short> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<unsigned short> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<int> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<unsigned int> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<long> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<unsigned long> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<float> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<double> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<>
    struct TypeTraits<long double> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };

    template<typename T>
    struct TypeTraits<T *> {
        using hasTrivialDefaultConstructor = TrueType;
        using hasTrivialCopyConstructor = TrueType;
        using hasTrivialAssignmentOperator = TrueType;
        using hasTrivialDestructor = TrueType;
        using isPODType = TrueType;
    };
}

#endif
