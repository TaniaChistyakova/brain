/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "Letter.h"

#include "TestTools.h"

#include "gtest/gtest.h"

TEST(LetterTest, LetterTypeTest)
{
    DotArray da = TestTools::GenerateRandomDotVector(16);
    Letter bits = 0;
    LetterType type;

    da[0] = 0;
    da[1] = 0;

    bits = da.ToBits<Letter>();
    type = LetterTools::Type(bits);

    EXPECT_EQ(LetterType::Ignored, type);

    da[0] = 1;
    da[1] = 0;

    bits = da.ToBits<Letter>();
    type = LetterTools::Type(bits);

    EXPECT_EQ(LetterType::Sentence, type);


    da[0] = 0;
    da[1] = 1;

    bits = da.ToBits<Letter>();
    type = LetterTools::Type(bits);

    EXPECT_EQ(LetterType::Functor, type);


    da[0] = 1;
    da[1] = 1;

    bits = da.ToBits<Letter>();
    type = LetterTools::Type(bits);

    EXPECT_EQ(LetterType::Word, type);
}

TEST(LetterTest, LetterSentenceTest)
{
    uint64_t sentenceLength = TestTools::GenerateRandomNumber(0, (int)(Pow2(SentenceObjLengthSize) - 1));

    Letter ltr = LetterTools::PutSentenceObjLength(sentenceLength);

    LetterType type = LetterTools::Type(ltr);
    EXPECT_EQ(LetterType::Sentence, type);

    uint64_t bits = LetterTools::GetSentenceObjLength(ltr);
    EXPECT_EQ(sentenceLength, bits);

    sentenceLength = std::numeric_limits<uint64_t>::max();
    ltr = LetterTools::PutSentenceObjLength(sentenceLength);

    bits = LetterTools::GetSentenceObjLength(ltr);
    EXPECT_EQ(SentenceMaxArity, bits);
}

TEST(LetterTest, LetterFunctorTest)
{
    uint64_t functorCode = TestTools::GenerateRandomNumber(0, 15);
    Letter ltr;
    LetterType type;

    ltr = LetterTools::PutFunctor(2, functorCode);

    type = LetterTools::Type(ltr);
    EXPECT_EQ(LetterType::Functor, type);

    uint64_t res = LetterTools::GetFunctorArity(ltr);
    EXPECT_EQ(2ull, res);

    res = LetterTools::GetFunctorCode(ltr, 2);
    EXPECT_EQ(functorCode, res);

    ltr = LetterTools::PutFunctor(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max());

    type = LetterTools::Type(ltr);
    EXPECT_EQ(LetterType::Functor, type);

    res = LetterTools::GetFunctorArity(ltr);
    EXPECT_EQ(3ull, res);

    res = LetterTools::GetFunctorCode(ltr, 3);
    EXPECT_EQ(255ull, res);

}

TEST(LetterTest, LetterWordTest)
{
    Word word = TestTools::GenerateRandomDotVector(WordSize);
    Letter ltr = LetterTools::PutWord(word.ToBits());

    LetterType type = LetterTools::Type(ltr);
    EXPECT_EQ(LetterType::Word, type);

    uint16_t wordBits = (uint16_t)LetterTools::GetWord(ltr);
    EXPECT_EQ(word.ToBits(), wordBits);
}

