#ifndef _BIT_VECTOR_
#define _BIT_VECTOR_
/**
 * https://github.com/TheLartians/BitLens
 *
 * MIT License
 *
 * Copyright (c) 2020 Lars Melchior
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <climits>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
#include <iostream>

template<class Word>
struct BitWordSize {
    static constexpr size_t value = sizeof(Word) * CHAR_BIT;
};

/*
 * A reference for a single bit
 */
template<class Word>
class BitReference {
private:
    size_t offset;
    Word &value;

public:
    constexpr BitReference(Word &v, size_t o) noexcept: offset(o), value(v) {
        static_assert(std::is_integral<Word>::value && std::is_unsigned<Word>::value,
                      "only unsigned integer value types are supported");
    }

    constexpr operator bool() const noexcept { return (value >> offset) & Word(1); }

    BitReference &operator=(bool v) noexcept {
        value = (value & ~(Word(1) << offset)) | (Word(v) << offset);
        return *this;
    }

    BitReference &operator=(const BitReference &v) noexcept { return *this = bool(v); }
};

/**
 * Swaps only the referenced bit
 */
template<class Word>
void swap(BitReference<Word> &a, BitReference<Word> &b) {
    bool tmp = a;
    a = b;
    b = tmp;
}

template<class ContainerIterator>
class BitIterator {
private:
    using Word = typename std::remove_reference<decltype(*std::declval<ContainerIterator>())>::type;
    static constexpr auto WORD_SIZE = BitWordSize<Word>::value;
    ContainerIterator iterator;
    size_t index;

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = BitReference<Word>;
    using difference_type = size_t;
    using reference = value_type;

    constexpr BitIterator(ContainerIterator i, size_t o) noexcept: iterator(i), index(o) {}

    BitIterator &operator++(int) noexcept {
        BitIterator copy(*this);
        index++;
        return copy;
    }

    BitIterator operator++() noexcept {
        ++index;
        return *this;
    }

    BitIterator &operator--(int) noexcept {
        BitIterator copy(*this);
        index--;
        return copy;
    }

    BitIterator operator--() noexcept {
        --index;
        return *this;
    }

    BitIterator operator+(int n) const noexcept { return BitIterator(iterator, index + n); }

    BitIterator operator-(int n) const noexcept { return BitIterator(iterator, index - n); }

    size_t operator-(const BitIterator &other) const noexcept { return index - other.index; }

    constexpr bool operator!=(const BitIterator &other) const {
        return iterator != other.iterator || index != other.index;
    }

    constexpr bool operator==(const BitIterator &other) const {
        return iterator == other.iterator && index == other.index;
    }

    constexpr BitReference<Word> operator*() const noexcept {
        return BitReference<Word>(*(iterator + index / WORD_SIZE), index % WORD_SIZE);
    }
};

template<class Container>
class BitLens {
private:
    Container &container;
    size_t len;
public:
    using Word = typename std::remove_reference<decltype(std::declval<Container>()[0])>::type;
    static constexpr auto WORD_SIZE = BitWordSize<Word>::value;

    constexpr BitLens(Container &c) noexcept: container(c), len(0) {
    }

    /**
     * returns the number of bits that fit in the container
     */
    size_t size() const noexcept(noexcept(container.size())) {
        return len;//container.size() * WORD_SIZE;
    }

    /**
     * resizes the container to hold at least `N` bits.
     * Note that the actual size will be a multiple of the bits in a word.
     */
    void resize(size_t N, bool v = false) noexcept(noexcept(container.resize(0, 0))) {
        Word offset = N % WORD_SIZE == 0 ? 0 : 1;
        container.resize(N / WORD_SIZE + offset, v ? ~0 : 0);
        len = N;
    }

//    [[deprecated("legacy API: use `resize(i,v)` insted")]]
//    void resizeToHold(size_t N, bool v = false) {
//        Word offset = N % WORD_SIZE == 0 ? 0 : 1;
//        container.resize(N / WORD_SIZE + offset, v ? ~0 : 0);
//    }

    /**
     * returns a BitReference to the bit at position `i`.
     */
    constexpr BitReference<Word> operator[](size_t i)
    noexcept(noexcept(container[0])) {
        Word offset = i % WORD_SIZE;
        return BitReference<Word>(container[i / WORD_SIZE], offset);
    }

    /**
     * returns a BitReference to the bit at position `i`.
     */
    constexpr BitReference<const Word> operator[](size_t i) const
    noexcept(noexcept(container[0])) {
        Word offset = i % WORD_SIZE;
        return BitReference<const Word>(container[i / WORD_SIZE], offset);
    }

//    [[deprecated("legacy API: use `[i]` insted")]]
//    bool get(size_t i) const
//    noexcept(noexcept(std::declval<BitLens<Container>>()[i])) {
//        return (*this)[i];
//    }

//    [[deprecated("legacy API: use `[i] = v` insted")]] void set(size_t i, bool v) noexcept(
//    noexcept(std::declval<BitLens<Container>>()[i])) {
//        (*this)[i] = v;
//    }

    constexpr auto begin() noexcept(noexcept(container.begin())) {
        return BitIterator(container.begin(), 0);
    }

    constexpr auto end() noexcept(noexcept(container.begin())) {
        return BitIterator(container.begin(), size());
    }

    constexpr auto begin() const noexcept(noexcept(container.begin())) {
        return BitIterator(container.begin(), 0);
    }

    constexpr auto end() const noexcept(noexcept(container.begin())) {
        return BitIterator(container.begin(), size());
    }

//    template<class F>
//    [[deprecated("legacy API: use iterators instead")]]
//    void forEach(F &&f) {
//        size_t N = size();
//        for (size_t idx = 0; idx != N; ++idx) {
//            f((*this)[idx], idx);
//        }
//    }

    /**
     *  get the underlying data container
     */
    Container &data() noexcept { return container; }

    /**
     *  get the underlying data container
     */
    const Container &data() const noexcept { return container; }

    void push(bool v) {
        if (len < container.size() * WORD_SIZE) {
            (*this)[len] = v;
            ++len;
        } else {
            ++len;
            resize(len, v);
        }
    }

    void setLen(int l) noexcept { len = l; }
};

///**
// * Legacy API
// * @deprecated Use BitLens instead
// */
//template<class BitContainer> using Lens = BitLens<BitContainer>;

/**
 * creates a bitlens that owns its data
 */
template<class C>
class BitContainer : public BitLens<C> {
private:
    C dataContainer;

public:
    void setData(C &c) {
        dataContainer = std::move(c);
    }

    constexpr BitContainer() noexcept(noexcept(C())): BitLens<C>(dataContainer) {}

    constexpr BitContainer(const BitContainer &other)
    noexcept(noexcept(C(std::declval<const C &>())))
            : dataContainer(other.data()), BitLens<C>(dataContainer) {}

    constexpr BitContainer(BitContainer &&other)
    noexcept(noexcept(C(std::declval<C &&>())))
            : dataContainer(std::move(other.data())), BitLens<C>(dataContainer) {}

    template<typename... Args>
    BitContainer(Args &&... args)
            : dataContainer(std::forward<Args...>(args...)), BitLens<C>(dataContainer) {}
};


namespace std {
    template<class T>
    struct iterator_traits<BitIterator<T>> : public BitIterator<T> {
    };
}  // namespace std

using BitVector = BitContainer<std::vector<unsigned char>>;

template<class C>
std::ostream &operator<<(std::ostream &os, const BitContainer<C> &vec) {
    auto bitLen = vec.size();
    os.write(reinterpret_cast<const char *>(&bitLen), sizeof(bitLen));
    auto containerLen = vec.data().size();
    os.write(reinterpret_cast<const char *>(&containerLen), sizeof(containerLen));
    for (auto &i:vec.data()) {
        os.write(reinterpret_cast<const char *>(&i), sizeof(i));
    }
    return os;
}

template<class C>
std::istream &operator>>(std::istream &is, BitContainer<C> &vec) {
    decltype(vec.size()) bitLen;
    is.read(reinterpret_cast<char *>(&bitLen), sizeof(bitLen));
    decltype(vec.data().size()) containerLen;
    is.read(reinterpret_cast<char *>(&containerLen), sizeof(containerLen));
    C c;
    for (decltype(vec.data().size()) i = 0; i < containerLen; i++) {
        c.resize(c.size() + 1);
        auto &ref = c[c.size() - 1];
        is.read(reinterpret_cast<char *>(&ref), sizeof(ref));
    }
    vec.setLen( bitLen);
    vec.setData(c);
    return is;
}

#endif// _BIT_VECTOR_