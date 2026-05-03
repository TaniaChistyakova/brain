/*-----------------------------------------------------
 *
 *  Copyright (c) 2026 Tania Chistyakova
 *
 *----------------------------------------------------- */

#pragma once

#include <vector>
#include <cassert>

class ShuffleIterator
{
public:
    ShuffleIterator(size_t movingIndices, size_t staticIndices)
        : n(movingIndices)
        , m(staticIndices)
        , multi_index(n, 0)
    {

    }


    std::vector<size_t> Output() const
    {
        std::vector<size_t> out;
        out.reserve(n + m);

        for (size_t j = 0; j < n; j++)
        {
            if (0 == multi_index[j])
            {
                out.push_back(j);
            }
        }

        for (size_t i = 0; i < m; i++)
        {
            out.push_back(n + i);
            for (size_t j = 0; j < n; j++)
            {
                if (i + 1 == multi_index[j])
                {
                    out.push_back(j);
                }
            }
        }

        return out;
    };

    bool Advance()
    {
        size_t i = 0;
        if (n == 0)
        {
            return false;
        }

        for (; i < n; i++)
        {
            size_t& index_value = multi_index[i];
            if (++index_value != m + 1)
            {
                break;
            }
            if (i < n - 1)
            {
                index_value = 0;
            }
            else
            {
                return false;
            }
        }
        return true;
    };

    template<typename T, template<class, class...> class Cont1, class... Args1, template<class, class...> class Cont2, class... Args2>
    static auto Shuffle(std::vector<size_t> const& indices, const Cont1<T, Args1...>& in, Cont2<T, Args2...>& out)
    {
        assert(indices.size() == in.size());

        out.resize(indices.size());
        for (size_t i = 0; i < indices.size(); i++)
        {
            size_t j = indices[i];

            assert(j < indices.size());

            out[j] = in[i];
        }
    }

private:
    size_t n;
    size_t m;
    std::vector<size_t> multi_index;
};
