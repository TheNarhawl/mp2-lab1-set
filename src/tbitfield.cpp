// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include <exception>
#include "tbitfield.h"


static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

// инициализация полей и вычисление необходимой памяти
TBitField::TBitField(int len) : BitLen(len)
{
    if (len < 0) {
        pMem = nullptr;
        BitLen = 0;
        MemLen = 0;
        throw exception();
    }

    MemLen = BitLen / (8 * sizeof(TELEM)) + 1;
    pMem = new TELEM[MemLen];

    for (int i = 0; i < MemLen; i++) {
        pMem[i] = (TELEM)0;
    }
}

TBitField::TBitField(const TBitField& buffer) // конструктор копирования
{
    BitLen = buffer.BitLen;
    MemLen = buffer.MemLen;

    if (MemLen == 0) {
        pMem = nullptr;
    }
    else {
        pMem = new TELEM[MemLen];

        copy(buffer.pMem, buffer.pMem + MemLen, pMem);
    }
}

TBitField::~TBitField() // деструктор
{
    BitLen = 0;
    MemLen = 0;

    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const  // индекс Мем для бита n
{
    if (n >= BitLen) throw exception();
    return n / (sizeof(TELEM) * 8);
}

TELEM TBitField::GetMemMask(const int n) const  // битовая маска для бита n
{
    return 1 << (n % (sizeof(TELEM) * 8));
}


// ДОСТУП К БИТАМ БИТОВОГО ПОЛЯ


int TBitField::GetLength(void) const // получить длину (кол-во бит)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n >= BitLen || n < 0) throw exception();
  

    int memPos = GetMemIndex(n);
    TELEM bitPos = GetMemMask(n);

    pMem[memPos] |= bitPos;
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if ((n < 0) | (n > BitLen)) throw exception();
   
    int memPos = GetMemIndex(n);
    TELEM bitPos = GetMemMask(n);

    pMem[memPos] &= ~bitPos;

}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if ((n < 0) | (n > BitLen)) throw exception();

    if ((pMem[GetMemIndex(n)] & GetMemMask(n)) > 0) return 1;
    else return 0;
}


// БИТОВЫЕ ОПЕРАЦИИ


TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
    if (this == &bf) return *this;

    BitLen = bf.BitLen;
    MemLen = bf.MemLen;

    pMem = new TELEM[MemLen];

    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }

    return *this;
}

int TBitField::operator==(const TBitField& bf) const // сравнение ==
{
    if (BitLen != bf.BitLen) return 0;

    for (int i = 0; i < BitLen; i++) {
        if (GetBit(i) != bf.GetBit(i)) return 0;
    }

    return 1;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение !=
{
    if (BitLen != bf.BitLen) return 1;
    for (int i = 0; i < BitLen; i++)
        if (GetBit(i) != bf.GetBit(i))
            return 1;
    return 0;
}

TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
    TBitField resultMaxLength(GetLength() >= bf.GetLength() ? *this : bf);
    for (int i = 0; i < (BitLen < bf.BitLen ? BitLen : bf.BitLen); i++) {
        if ((GetBit(i) | bf.GetBit(i)) > 0) resultMaxLength.SetBit(i);
    }

    return resultMaxLength;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"
{
    int max_len = BitLen > bf.BitLen ? BitLen : bf.BitLen;
    int min_len = BitLen + bf.BitLen - max_len;

    TBitField resultMaxLength(max_len);

    for (int i = 0; i < min_len; i++) {
        if (GetBit(i) & bf.GetBit(i)) resultMaxLength.SetBit(i);
        else resultMaxLength.ClrBit(i);
    }

    for (int i = min_len; i < max_len; i++) {
        resultMaxLength.ClrBit(i);
    }

    return resultMaxLength;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField result(BitLen);
    for (int i = 0; i < BitLen; i++) {
        if (GetBit(i)) result.ClrBit(i);
        else result.SetBit(i);
    }

    return result;
}


// ВВОД / ВЫВОД


istream& operator>>(istream& istr, TBitField& bf) // ввод
{
    // в переменную sign записывается значение из потока istr
    int sign;

    for (int i = 0; i < bf.BitLen; i++) {
        istr >> sign;
        if (sign == 1) {
            bf.SetBit(bf.BitLen - i - 1);
        }
        else if (sign == 0) {
            bf.ClrBit(bf.BitLen - i - 1);
        }
        else throw exception();
    }
    return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
    for (size_t i = 0; i < bf.MemLen; i++) {
        ostr << bf.pMem[i];
    }

    return ostr;
}
