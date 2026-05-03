/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#include "LetterView.h"

#include "TestTools.h"

#include "gtest/gtest.h"


TEST(LetterViewTest, LetterViewOnVector)
{
    std::vector<Letter> vec = { 1,2,3,4,5,6,7,8,9 };
    LetterView<std::vector<Letter> > view(vec, 2, 5);

    EXPECT_EQ(3ull, view.sizeInLetters());
    EXPECT_EQ(3ull * 16ull, view.size());
    EXPECT_EQ((Letter)3, view[0]);
    EXPECT_EQ((Letter)6, view[3]);

    LetterView<std::vector<Letter> > view2(vec, 2);

    EXPECT_EQ(7ull, view2.sizeInLetters());
    EXPECT_EQ(7ull * 16ull, view2.size());
    EXPECT_EQ((Letter)3, view2[0]);
    EXPECT_EQ((Letter)9, view2[6]);
}

TEST(LetterViewTest, LetterViewOnDotArray)
{
    size_t szArray = 16 * 10 + 2;
    DotArray da = TestTools::GenerateRandomDotVector(szArray);

    LetterView<DotArray> view1(da);

    EXPECT_EQ(11ull, view1.sizeInLetters());
    EXPECT_EQ(szArray, view1.size());

    LetterView<DotArray> view2(da, 1);

    EXPECT_EQ(10ull, view2.sizeInLetters());
    EXPECT_EQ(szArray - 16ull, view2.size());

    LetterView<DotArray> view3(da, 2, 4);

    EXPECT_EQ(2ull, view3.sizeInLetters());
    EXPECT_EQ(2ull * 16ull, view3.size());


}

