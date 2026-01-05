/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2015 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "unit_test_framework.h"

#include <string>
#include <ostream>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

#include "etl/optional.h"
#include "etl/vector.h"
#include "data.h"

typedef TestDataNDC<std::string> Data;
typedef TestDataM<uint32_t>      DataM;

std::ostream& operator << (std::ostream& os, const Data& data)
{
  os << data.value;
  return os;
}

std::ostream& operator << (std::ostream& os, const etl::optional<Data>& data)
{
  os << data.value();
  return os;
}

namespace
{
#include "etl/private/diagnostic_unused_function_push.h"
  //*************************************************************************
  struct NonTrivial
  {
    constexpr NonTrivial() : a(0) {}

    constexpr NonTrivial(int a_) : a(a_) {}

    constexpr friend bool operator <(const NonTrivial& lhs, const NonTrivial& rhs)
    {
      return lhs.a < rhs.a;
    }

    constexpr friend bool operator ==(const NonTrivial& lhs, const NonTrivial& rhs)
    {
      return lhs.a == rhs.a;
    }

    int a;
  };
#include "etl/private/diagnostic_pop.h"

#if ETL_USING_CPP11
    class Fallible final
    {
    public:
      explicit Fallible(const bool _should_throw = false) noexcept(false)
        : should_throw{_should_throw}
      {
        throw_once();
      }
      Fallible(const Fallible& other) noexcept(false)
        : should_throw{other.should_throw}
      {
        throw_once();
      }
      Fallible& operator=(const Fallible& other) noexcept(false)
      {
        should_throw = other.should_throw;
        throw_once();
        return *this;
      }
      Fallible(Fallible&& other) noexcept(false)
        : should_throw{other.should_throw}
      {
        throw_once();
      }
      Fallible& operator=(Fallible&& other) noexcept(false)
      {
        should_throw = other.should_throw;
        throw_once();
        return *this;
      }
      ~Fallible() noexcept  // NOLINT Non-trivial destructor on purpose
      {
      }
    private:
      bool should_throw;

      void throw_once() noexcept(false)
      {
        if (should_throw)
        {
          should_throw = false;
          throw std::bad_exception();
        }
      }
    };
    ETL_STATIC_ASSERT(std::is_default_constructible<Fallible>::value, "`Fallible` is default constructible");
    ETL_STATIC_ASSERT(std::is_copy_constructible<Fallible>::value, "`Fallible` is copy constructible");
    ETL_STATIC_ASSERT(std::is_move_constructible<Fallible>::value, "`Fallible` is move constructible");
    ETL_STATIC_ASSERT(std::is_copy_assignable<Fallible>::value, "`Fallible` is copy assignable");
    ETL_STATIC_ASSERT(std::is_move_assignable<Fallible>::value, "`Fallible` is move assignable");
    ETL_STATIC_ASSERT(!noexcept(Fallible{}), "`Fallible{}` throws std::bad_exception()");
    ETL_STATIC_ASSERT(!std::is_nothrow_copy_constructible<Fallible>::value, "`Fallible` copy constructor throws");
    ETL_STATIC_ASSERT(!std::is_nothrow_move_constructible<Fallible>::value, "`Fallible` move constructor throws");
    ETL_STATIC_ASSERT(!std::is_nothrow_copy_assignable<Fallible>::value, "`Fallible` copy assignment throws");
    ETL_STATIC_ASSERT(!std::is_nothrow_move_assignable<Fallible>::value, "`Fallible` move assignment throws");

    class Infallible final
    {
    public:
      Infallible() noexcept {}  // NOLINT Non-trivial on purpose
      Infallible(const Infallible&) noexcept {}  // NOLINT Non-trivial on purpose
      Infallible(Infallible&&) noexcept {}
      Infallible& operator=(const Infallible&) noexcept { return *this; }  // NOLINT Non-trivial on purpose
      Infallible& operator=(Infallible&&) noexcept { return *this; }
      ~Infallible() noexcept {}  // NOLINT Non-trivial on purpose
    };
    ETL_STATIC_ASSERT(std::is_default_constructible<Infallible>::value, "`Infallible` is default constructible");
    ETL_STATIC_ASSERT(std::is_copy_constructible<Infallible>::value, "`Infallible` is copy constructible");
    ETL_STATIC_ASSERT(std::is_move_constructible<Infallible>::value, "`Infallible` is move constructible");
    ETL_STATIC_ASSERT(std::is_copy_assignable<Infallible>::value, "`Infallible` is copy assignable");
    ETL_STATIC_ASSERT(std::is_move_assignable<Infallible>::value, "`Infallible` is move assignable");
    ETL_STATIC_ASSERT(noexcept(Infallible{}), "`Infallible{}` never throws");
    ETL_STATIC_ASSERT(std::is_nothrow_copy_constructible<Infallible>::value, "`Infallible` copy constructor never throws");
    ETL_STATIC_ASSERT(std::is_nothrow_move_constructible<Infallible>::value, "`Infallible` move constructor never throws");
    ETL_STATIC_ASSERT(std::is_nothrow_copy_assignable<Infallible>::value, "`Infallible` copy assignment never throws");
    ETL_STATIC_ASSERT(std::is_nothrow_move_assignable<Infallible>::value, "`Infallible` move assignment never throws");
#endif

  SUITE(test_optional)
  {
    //*************************************************************************
    TEST(test_initialisation)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(!bool(data1));
      CHECK(!bool(data2));
      CHECK(!data1.has_value());
      CHECK(!data2.has_value());

      data1 = Data("Hello");
      CHECK(bool(data1));
      CHECK(data1.has_value());
      CHECK_EQUAL(Data("Hello"), data1);

      data1 = data2;
      CHECK(!bool(data1));
      CHECK(!bool(data2));
      CHECK(!data1.has_value());
      CHECK(!data2.has_value());

      data1 = Data("World");
      data2 = data1;
      CHECK(bool(data1));
      CHECK(bool(data2));
      CHECK(data1.has_value());
      CHECK(data2.has_value());

      etl::optional<Data> data3(data1);
      CHECK(bool(data3));
      CHECK(data3.has_value());
      CHECK_EQUAL(data1, data3);

      etl::optional<Data> data4;
      data4 = Data("Hello");
      data4 = etl::nullopt;
      CHECK(!bool(data4));
      CHECK(!data4.has_value());
    }

#if ETL_USING_CPP11
    // `constexpr optional() noexcept`
    TEST(test_assert_noexcept_of_default_constructor_cpp11)
    {
      ETL_STATIC_ASSERT(
        std::is_default_constructible<etl::optional<int>>::value,
        "`etl::optional<int>` is default constructible");
      ETL_STATIC_ASSERT(
        std::is_default_constructible<etl::optional<Fallible>>::value,
        "`etl::optional<Fallible>` is default constructible");
      ETL_STATIC_ASSERT(
        std::is_default_constructible<etl::optional<Infallible>>::value,
        "`etl::optional<Infallible>` is default constructible");

      ETL_STATIC_ASSERT(
        noexcept(etl::optional<int>{}),
        "No throw `elt::optional<T>{}` for trivial `T{}`"
        );
      ETL_STATIC_ASSERT(
        noexcept(etl::optional<Fallible>{}),
        "No throw `elt::optional<T>{}` even for fallible `T{}`"
        );
      ETL_STATIC_ASSERT(
        noexcept(etl::optional<Infallible>{}),
        "No throw `elt::optional<T>{}` for infallible `T{}`"
        );

      // Compare with STL
      #if ETL_USING_CPP17
      ETL_STATIC_ASSERT(
        noexcept(std::optional<int>{}),
        "`std::optional<T>{}` has the same no throw behavior as ETL, where T is fundamental"
        );
      ETL_STATIC_ASSERT(
        noexcept(std::optional<Fallible>{}),
        "`std::optional<T>{}` has the same no throw behavior as ETL, even for fallible `T{}`"
        );
      ETL_STATIC_ASSERT(
        noexcept(std::optional<Infallible>{}),
        "`std::optional<T>{}` has the same no throw behavior as ETL, where `T{}` is non-throwable"
      );
      #endif
    }

    // `constexpr optional(nullopt_t) noexcept`
    TEST(test_assert_noexcept_of_nullopt_constructor_cpp11)
    {
      ETL_STATIC_ASSERT(
        (std::is_constructible<etl::optional<int>, etl::nullopt_t>::value),
        "`etl::optional<int>` is nullopt constructible");
      ETL_STATIC_ASSERT(
        (std::is_constructible<etl::optional<Fallible>, etl::nullopt_t>::value),
        "`etl::optional<Fallible>` is nullopt constructible");
      ETL_STATIC_ASSERT(
        (std::is_constructible<etl::optional<Infallible>, etl::nullopt_t>::value),
        "`etl::optional<Infallible>` is nullopt constructible");

      ETL_STATIC_ASSERT(
        noexcept(etl::optional<int>{etl::nullopt}),
        "No throw for `elt::optional<T>{etl::nullopt}` where T is fundamental"
        );
      ETL_STATIC_ASSERT(
        noexcept(etl::optional<Fallible>{etl::nullopt}),
        "No throw for `elt::optional<T>{etl::nullopt}` where T is fallible"
        );
      ETL_STATIC_ASSERT(
        noexcept(etl::optional<Infallible>{etl::nullopt}),
        "No throw for `elt::optional<T>{etl::nullopt}` where T is infallible"
        );

      // Compare with STL
      #if ETL_USING_CPP17
      static_assert(
        noexcept(std::optional<int>{std::nullopt}),
        "`std::optional<T>{std::nullopt}` has the same no throw behavior as ETL, where T is fundamental"
        );
      static_assert(
        noexcept(std::optional<Fallible>{std::nullopt}),
        "`std::optional<T>{std::nullopt}` has the same no throw behavior as ETL, where T is fallible"
        );
      static_assert(
        noexcept(std::optional<Infallible>{std::nullopt}),
        "`std::optional<T>{std::nullopt}` has the same no throw behavior as ETL, where T is infallible"
        );
      #endif
    }

    // `constexpr optional(optional&& other) noexcept(conditional)`
    TEST(test_assert_noexcept_of_move_constructor_cpp11)
    {
      ETL_STATIC_ASSERT(
        std::is_move_constructible<etl::optional<int>>::value,
        "`etl::optional<int>` is move constructible");
      ETL_STATIC_ASSERT(
        std::is_move_constructible<etl::optional<Fallible>>::value,
        "`etl::optional<Fallible>` is move constructible");
      ETL_STATIC_ASSERT(
        std::is_move_constructible<etl::optional<Infallible>>::value,
        "`etl::optional<Infallible>` is move constructible");

      ETL_STATIC_ASSERT(
        std::is_nothrow_move_constructible<etl::optional<int>>::value,
        "`etl::optional<int>` move constructor never throws"
        );
      ETL_STATIC_ASSERT(
        !std::is_nothrow_move_constructible<etl::optional<Fallible>>::value,
        "`etl::optional<Fallible>` move constructor might throw"
        );
      ETL_STATIC_ASSERT(
        std::is_nothrow_move_constructible<etl::optional<Infallible>>::value,
        "`etl::optional<Infallible>` move constructor never throws"
        );

      // Compare with STL
      #if ETL_USING_CPP17
      static_assert(
        std::is_nothrow_move_constructible_v<std::optional<int>>,
        "`std::optional<int>` move constructor never throws"
        );
      static_assert(
        !std::is_nothrow_move_constructible_v<std::optional<Fallible>>,
        "`std::optional<Fallible>` move constructor might throw"
        );
      static_assert(
        std::is_nothrow_move_constructible_v<std::optional<Infallible>>,
        "`std::optional<Infallible>` move constructor never throws"
        );
      #endif
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_emplace_construction_cpp14)
    {
      constexpr etl::optional<int> opt(etl::in_place_t{}, 1);

      CHECK_TRUE(opt.has_value());
      CHECK(bool(opt));
      CHECK_EQUAL(1, opt.value());
    }
#endif

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_emplace_construction_cpp20)
    {
      struct TestData
      {
        constexpr TestData(int a_, int b_)
          : a(a_), b(b_)
        {
        }

        int a;
        int b;
      };

      constexpr etl::optional<TestData> opt(etl::in_place_t{}, 1, 2);

      CHECK_TRUE(opt.has_value());
      CHECK(bool(opt));
      CHECK_EQUAL(1, opt.value().a);
      CHECK_EQUAL(2, opt.value().b);
    }
#endif

    //*************************************************************************
    TEST(test_construct_from_initializer_list_and_arguments)
    {
      struct S
      {
        S()
          : vi()
          , a(0)
          , b(0)
        {
        }

        S(std::initializer_list<int> il, int a_, int b_)
          : vi(il)
          , a(a_)
          , b(b_)
        {
        }

        std::vector<int> vi;
        int a;
        int b;
      };

      etl::optional<S> opt(etl::in_place_t{}, { 10, 11, 12 }, 1, 2);

      CHECK_EQUAL(10, opt.value().vi[0]);
      CHECK_EQUAL(11, opt.value().vi[1]);
      CHECK_EQUAL(12, opt.value().vi[2]);
      CHECK_EQUAL(1, opt.value().a);
      CHECK_EQUAL(2, opt.value().b);
    }

    //*************************************************************************
    TEST(test_deduced_initialisation)
    {
      Data data("Hello");

      etl::optional<Data> opt{ data };

      CHECK(opt.has_value());
      CHECK(bool(opt));
      CHECK_EQUAL(data, opt);
    }

    //*************************************************************************
    TEST(test_emplace)
    {
      etl::optional<DataM> data;

      data.emplace(1U);
      CHECK_EQUAL(1U, data.value().value);

      data.emplace(2U);
      CHECK_EQUAL(2U, data.value().value);

      data.emplace(3U);
      CHECK_EQUAL(3U, data.value().value);

      CHECK_EQUAL(1, DataM::get_instance_count());
    }

    //*************************************************************************
    TEST(test_emplace_zero_parameters_fundamental)
    {
      etl::optional<std::uint8_t> result = 1;
      CHECK_EQUAL(0, static_cast<int>(result.emplace()));

      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(0, int(result.value()));
    }

    //*************************************************************************
    TEST(test_emplace_zero_parameters_non_fundamental)
    {
      etl::optional<std::string> result = std::string("abc");

      std::string& ref = result.emplace();
      CHECK_EQUAL(std::string(), ref);
      CHECK_EQUAL(&ref, &result.value());
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL("", std::string(result.value()));
    }

    //*************************************************************************
    TEST(test_emplace_return)
    {
      etl::optional<DataM> data;

      DataM* datam = &data.emplace(1U);
      CHECK_EQUAL(datam, &data.value());
      CHECK(datam != nullptr);
    }

    //*************************************************************************
    TEST(test_moveable)
    {
#include "etl/private/diagnostic_pessimizing_move_push.h"
      etl::optional<DataM> data(std::move(DataM(1)));
      CHECK_EQUAL(1U, data.value().value);
      CHECK(bool(data));

      data = std::move(etl::optional<DataM>(std::move(DataM(2))));
      CHECK_EQUAL(2U, data.value().value);
      CHECK(bool(data));

      etl::optional<DataM> data2(etl::move(data));
      CHECK_EQUAL(2U, data2.value().value);
      CHECK(bool(data2));
#include "etl/private/diagnostic_pop.h"
    }

    //*************************************************************************
    TEST(test_nullopt_pod)
    {
      etl::optional<int> data(etl::nullopt);
      data = 1;
      data = etl::nullopt;
      CHECK(!bool(data));
    }

    //*************************************************************************
    TEST(test_nullopt)
    {
      etl::optional<Data> data(etl::nullopt);
      data = Data("Hello");
      data = etl::nullopt;
      CHECK(!bool(data));
    }

    //*************************************************************************
    TEST(test_value_or)
    {
      etl::optional<Data> data;

      Data result = data.value_or(Data("Default"));
      CHECK_EQUAL(Data("Default"), result);

      data = Data("Value");
      result = data.value_or(Data("Default"));
      CHECK_EQUAL(Data("Value"), result);
    }

    //*************************************************************************
    TEST(test_value_or_const)
    {
      using FundamentalType = int;
      using NonFundamentalType = std::string;

      const FundamentalType constFT{ 5 };
      int resultFT = etl::optional<FundamentalType>{}.value_or(constFT);
      CHECK_EQUAL(5, resultFT);

      const NonFundamentalType constNFT{ "Default" };
      NonFundamentalType resultNFT = etl::optional<NonFundamentalType>{}.value_or(constNFT);
      CHECK_EQUAL("Default", resultNFT);
    }

    //*************************************************************************
    struct github_bug_720_bug_helper
    {
      int value{ 5 };

      etl::optional<int> get_valid() const
      {
        return value;
      }

      etl::optional<int> get_invalid() const
      {
        return etl::optional<int>();
      }
    };

    TEST(test_chained_value_or_github_bug_720 )
    {
      github_bug_720_bug_helper helper {};

      int value1 = helper.get_valid().value_or(1);
      CHECK_EQUAL(5, value1);

      int value2 = helper.get_invalid().value_or(1);
      CHECK_EQUAL(1, value2);
    }

    //*************************************************************************
    TEST(test_equality)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(data1 == data2);
      CHECK(data2 == data1);

      data1 = Data("Data1");
      CHECK(!(data1 == data2));
      CHECK(!(data2 == data1));

      data1 = etl::nullopt;
      data2 = Data("Data2");
      CHECK(!(data1 == data2));
      CHECK(!(data2 == data1));

      data1 = Data("Data1");
      data2 = Data("Data1");
      CHECK(data1 == data2);
      CHECK(data2 == data1);

      data1 = Data("Data1");
      data2 = Data("Data2");
      CHECK(!(data1 == data2));
      CHECK(!(data2 == data1));

      CHECK(!(etl::nullopt == data2));
      CHECK(!(data1 == etl::nullopt));

      CHECK(data1 == Data("Data1"));
      CHECK(!(data1 == Data("Data2")));
      CHECK(Data("Data1") == data1);
      CHECK(!(Data("Data2") == data1));
    }

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_constexpr_equality_non_trivial)
    {
      constexpr etl::optional<NonTrivial> data1(1);
      constexpr etl::optional<NonTrivial> data2(2);

      constexpr bool eq1 = (data1 == data2);
      constexpr bool eq2 = (data1 == etl::nullopt);
      constexpr bool eq3 = (etl::nullopt == data1);

      CHECK_FALSE(eq1);
      CHECK_FALSE(eq2);
      CHECK_FALSE(eq3);
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_constexpr_equality_trivial)
    {
      constexpr etl::optional<int> data1(1);
      constexpr etl::optional<int> data2(2);

      constexpr bool eq1 = (data1 == data2);
      constexpr bool eq2 = (data1 == etl::nullopt);
      constexpr bool eq3 = (etl::nullopt == data1);

      CHECK_FALSE(eq1);
      CHECK_FALSE(eq2);
      CHECK_FALSE(eq3);
    }
#endif

    //*************************************************************************
    TEST(test_inequality)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(!(data1 != data2));
      CHECK(!(data2 != data1));

      data1 = Data("Data1");
      CHECK(data1 != data2);
      CHECK(data2 != data1);

      data1 = etl::nullopt;
      data2 = Data("Data2");
      CHECK(data1 != data2);
      CHECK(data2 != data1);

      data1 = Data("Data1");
      data2 = Data("Data1");
      CHECK(!(data1 != data2));
      CHECK(!(data2 != data1));

      data1 = Data("Data1");
      data2 = Data("Data2");
      CHECK(data1 != data2);
      CHECK(data2 != data1);

      CHECK(etl::nullopt != data2);
      CHECK(data1 != etl::nullopt);

      CHECK(!(data1 != Data("Data1")));
      CHECK(data1 != Data("Data2"));
      CHECK(!(Data("Data1") != data1));
      CHECK(Data("Data2") != data1);
    }

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_constexpr_inequality_non_trivial)
    {
      constexpr etl::optional<NonTrivial> data1(1);
      constexpr etl::optional<NonTrivial> data2(2);

      constexpr bool eq1 = (data1 != data2);
      constexpr bool eq2 = (data1 != etl::nullopt);
      constexpr bool eq3 = (etl::nullopt != data1);

      CHECK_TRUE(eq1);
      CHECK_TRUE(eq2);
      CHECK_TRUE(eq3);
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_constexpr_inequality_trivial)
    {
      constexpr etl::optional<int> data1(1);
      constexpr etl::optional<int> data2(2);

      constexpr bool neq1 = (data1 != data2);
      constexpr bool neq2 = (data1 != etl::nullopt);
      constexpr bool neq3 = (etl::nullopt != data1);

      CHECK_TRUE(neq1);
      CHECK_TRUE(neq2);
      CHECK_TRUE(neq3);
    }
#endif

    //*************************************************************************
#include "etl/private/diagnostic_uninitialized_push.h"
    TEST(test_less_than)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(!(data2 < data1));
      CHECK(!(data1 < data2));

      data1 = Data("Data1");
      CHECK(!(data1 < data2));
      CHECK(data2 < data1);

      data1 = etl::nullopt;
      data2 = Data("Data2");
      CHECK(data1 < data2);
      CHECK(!(data2 < data2));

      data1 = Data("Data1");
      data2 = Data("Data2");
      CHECK(data1 < data2);
      CHECK(!(data2 < data1));

      CHECK(etl::nullopt < data2);
      CHECK(!(data1 < etl::nullopt));

      CHECK(data1 < Data("Data2"));
      CHECK(!(data1 < Data("Data1")));
      CHECK(!(Data("Data2") < data1));
      CHECK(Data("Data1") < data2);
    }
#include "etl/private/diagnostic_pop.h"


#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_constexpr_less_than_non_trivial)
    {
      constexpr etl::optional<NonTrivial> data1(1);
      constexpr etl::optional<NonTrivial> data2(2);

      constexpr bool lt1 = (data1 < data2);
      constexpr bool lt2 = (data2 < data1);
      constexpr bool lt3 = (data1 < etl::nullopt);
      constexpr bool lt4 = (etl::nullopt < data1);

      CHECK_TRUE(lt1);
      CHECK_FALSE(lt2);
      CHECK_FALSE(lt3);
      CHECK_TRUE(lt4);
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_constexpr_less_than_trivial)
    {
      constexpr etl::optional<int> data1(1);
      constexpr etl::optional<int> data2(2);

      constexpr bool lt1 = (data1 < data2);
      constexpr bool lt2 = (data2 < data1);
      constexpr bool lt3 = (data1 < etl::nullopt);
      constexpr bool lt4 = (etl::nullopt < data1);

      CHECK_TRUE(lt1);
      CHECK_FALSE(lt2);
      CHECK_FALSE(lt3);
      CHECK_TRUE(lt4);
    }
#endif

    //*************************************************************************
    TEST(test_less_than_equal)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(data1 <= data2);
      CHECK(data2 <= data1);

      data1 = Data("Data1");
      CHECK(!(data1 <= data2));
      CHECK(data2 <= data1);

      data1 = etl::nullopt;
      data2 = Data("Data2");
      CHECK(data1 <= data2);
      CHECK(!(data2 <= data1));

      data1 = Data("Data1");
      data2 = Data("Data2");
      CHECK(data1 <= data2);
      CHECK(!(data2 <= data1));

      CHECK(etl::nullopt <= data2);
      CHECK(!(data1 <= etl::nullopt));

      CHECK(data1 <= Data("Data2"));
      CHECK(!(data2 <= Data("Data1")));
      CHECK(data1 <= Data("Data1"));
      CHECK(!(Data("Data2") <= data1));
      CHECK(Data("Data1") <= data2);
      CHECK(Data("Data1") <= data1);
    }

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_constexpr_less_than_equal_non_trivial)
    {
      constexpr etl::optional<NonTrivial> data1(1);
      constexpr etl::optional<NonTrivial> data2(2);

      constexpr bool lt1 = (data1 <= data2);
      constexpr bool lt2 = (data2 <= data1);
      constexpr bool lt3 = (data1 <= etl::nullopt);
      constexpr bool lt4 = (etl::nullopt <= data1);

      CHECK_TRUE(lt1);
      CHECK_FALSE(lt2);
      CHECK_FALSE(lt3);
      CHECK_TRUE(lt4);
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_constexpr_less_than_equal_trivial)
    {
      constexpr etl::optional<int> data1(1);
      constexpr etl::optional<int> data2(2);

      constexpr bool lt1 = (data1 <= data2);
      constexpr bool lt2 = (data2 <= data1);
      constexpr bool lt3 = (data1 <= etl::nullopt);
      constexpr bool lt4 = (etl::nullopt <= data1);

      CHECK_TRUE(lt1);
      CHECK_FALSE(lt2);
      CHECK_FALSE(lt3);
      CHECK_TRUE(lt4);
    }
#endif

    //*************************************************************************
    TEST(test_greater_than)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(!(data1 > data2));
      CHECK(!(data2 > data1));

      data1 = Data("Data1");
      CHECK(data1 > data2);
      CHECK(!(data2 > data1));

      data1 = etl::nullopt;
      data2 = Data("Data2");
      CHECK(!(data1 > data2));
      CHECK(data2 > data1);

      data1 = Data("Data1");
      data2 = Data("Data2");
      CHECK(data2 > data1);
      CHECK(!(data1 > data2));

      CHECK(!(etl::nullopt > data2));
      CHECK(data1 > etl::nullopt);

      CHECK(!(data1 > Data("Data2")));
      CHECK(data2 > Data("Data1"));
      CHECK(Data("Data2") > data1);
      CHECK(!(Data("Data1") > data2));
    }

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_constexpr_greater_than_non_trivial)
    {
      constexpr etl::optional<NonTrivial> data1(1);
      constexpr etl::optional<NonTrivial> data2(2);

      constexpr bool gt1 = (data1 > data2);
      constexpr bool gt2 = (data2 > data1);
      constexpr bool gt3 = (data1 > etl::nullopt);
      constexpr bool gt4 = (etl::nullopt > data1);

      CHECK_FALSE(gt1);
      CHECK_TRUE(gt2);
      CHECK_TRUE(gt3);
      CHECK_FALSE(gt4);
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_constexpr_greater_than_trivial)
    {
      constexpr etl::optional<int> data1(1);
      constexpr etl::optional<int> data2(2);

      constexpr bool gt1 = (data1 > data2);
      constexpr bool gt2 = (data2 > data1);
      constexpr bool gt3 = (data1 > etl::nullopt);
      constexpr bool gt4 = (etl::nullopt > data1);

      CHECK_FALSE(gt1);
      CHECK_TRUE(gt2);
      CHECK_TRUE(gt3);
      CHECK_FALSE(gt4);
    }
#endif

    //*************************************************************************
    TEST(test_greater_than_equal)
    {
      etl::optional<Data> data1;
      etl::optional<Data> data2;

      CHECK(data1 >= data2);
      CHECK(data2 >= data1);

      data1 = Data("Data1");
      CHECK(data1 >= data2);
      CHECK(!(data2 >= data1));

      data1 = etl::nullopt;
      data2 = Data("Data2");
      CHECK(!(data1 >= data2));
      CHECK(data2 >= data1);

      data1 = Data("Data1");
      data2 = Data("Data2");
      CHECK(!(data1 >= data2));
      CHECK(data2 >= data1);

      CHECK(!(etl::nullopt >= data2));
      CHECK(data1 >= etl::nullopt);

      CHECK(!(data1 >= Data("Data2")));
      CHECK(data2 >= Data("Data1"));
      CHECK(data1 >= Data("Data1"));
      CHECK(Data("Data2") >= data1);
      CHECK(!(Data("Data1") >= data2));
      CHECK(Data("Data1") >= data1);
    }

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_constexpr_greater_than_equal_non_trivial)
    {
      constexpr etl::optional<NonTrivial> data1(1);
      constexpr etl::optional<NonTrivial> data2(2);

      constexpr bool gte1 = (data1 >= data2);
      constexpr bool gte2 = (data2 >= data1);
      constexpr bool gte3 = (data1 >= etl::nullopt);
      constexpr bool gte4 = (etl::nullopt >= data1);

      CHECK_FALSE(gte1);
      CHECK_TRUE(gte2);
      CHECK_TRUE(gte3);
      CHECK_FALSE(gte4);
    }
#endif

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_constexpr_greater_than_equal_trivial)
    {
      constexpr etl::optional<int> data1(1);
      constexpr etl::optional<int> data2(2);

      constexpr bool gte1 = (data1 >= data2);
      constexpr bool gte2 = (data2 >= data1);
      constexpr bool gte3 = (data1 >= etl::nullopt);
      constexpr bool gte4 = (etl::nullopt >= data1);

      CHECK_FALSE(gte1);
      CHECK_TRUE(gte2);
      CHECK_TRUE(gte3);
      CHECK_FALSE(gte4);
    }
#endif

    //*************************************************************************
    TEST(test_container_of_optional)
    {
      etl::vector<etl::optional<Data>, 10> container;

      container.resize(5, Data("1"));

      CHECK(bool(container[0]));
      CHECK(bool(container[1]));
      CHECK(bool(container[2]));
      CHECK(bool(container[3]));
      CHECK(bool(container[4]));
    }

    //*************************************************************************
    TEST(test_optional_container)
    {
      // The indexed access doesn't work in Linux for some reason!!!
#ifndef ETL_PLATFORM_LINUX
      etl::optional<etl::vector<Data, 10>> container;
      CHECK(!bool(container));//

      container = etl::vector<Data, 10>();
      CHECK(bool(container));

      container.value().resize(5, Data("1"));
      CHECK_EQUAL(5U, container.value().size());

      CHECK_EQUAL(Data("1"), container.value()[0]);
      CHECK_EQUAL(Data("1"), container.value()[1]);
      CHECK_EQUAL(Data("1"), container.value()[2]);
      CHECK_EQUAL(Data("1"), container.value()[3]);
      CHECK_EQUAL(Data("1"), container.value()[4]);
#endif
    }

    //*************************************************************************
    TEST(test_exception)
    {
      etl::optional<Data> data1;

      CHECK_THROW(data1.value(), etl::optional_invalid);
    }

    //*************************************************************************
    TEST(test_swap)
    {
      etl::optional<Data> original1(Data("1"));
      etl::optional<Data> original2(Data("2"));

      etl::optional<Data> data1;
      etl::optional<Data> data2;

      // Both invalid.
      swap(data1, data2);
      CHECK(!bool(data1));
      CHECK(!bool(data2));

      // Data1 valid;
      data1 = original1;
      data2 = etl::nullopt;
      swap(data1, data2);
      CHECK(!bool(data1));
      CHECK(bool(data2));
      CHECK_EQUAL(data2, original1);

      // Data2 valid;
      data1 = etl::nullopt;
      data2 = original2;
      swap(data1, data2);
      CHECK(bool(data1));
      CHECK(!bool(data2));
      CHECK_EQUAL(data1, original2);

      // Both valid;
      data1 = original1;
      data2 = original2;
      swap(data1, data2);
      CHECK(bool(data1));
      CHECK(bool(data2));
      CHECK_EQUAL(data1, original2);
      CHECK_EQUAL(data2, original1);
    }

    //*************************************************************************
    TEST(test_reset)
    {
      etl::optional<Data> data(Data("1"));
      CHECK(bool(data));

      data.reset();
      CHECK(!bool(data));
    }

    //*************************************************************************
    etl::optional<std::uint8_t> get_optional_test_bug_634()
    {
      etl::optional<std::uint8_t> result = 8;
      result.reset();

      return result;
    }

    //*************************************************************************
    TEST(test_bug_634)
    {
      etl::optional<std::uint8_t> result;

      result = get_optional_test_bug_634();

      CHECK_EQUAL(false, result.has_value());
    }

    //*************************************************************************
    TEST(test_optional_emplace_bug_636)
    {
      etl::optional<std::uint8_t> result = 1;
      result.emplace(2);

      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(2, result.value());
    }

    //*************************************************************************
    struct MyPODObject
    {
      MyPODObject() = delete;
      int value;
    };

    TEST(test_optional_pod_emplace_bug_712)
    {
      etl::optional<MyPODObject> optionalObject; // The Test: Does this compile for an object with a deleted default constructor?

      // Make sure it isn't optimised away.
      CHECK_FALSE(optionalObject.has_value());
    }

    //*************************************************************************
    TEST(test_optional_pod_assign_bug_714)
    {
      etl::optional<int> opt = 42;
      opt = etl::nullopt;

      CHECK_EQUAL(false, opt.has_value());
    }

    //*************************************************************************
    TEST(test_dereference_operator_bug_730)
    {
      etl::optional<int> opt = 42;

      CHECK_EQUAL(42, *opt);
    }

    //*************************************************************************
    TEST(test_const_dereference_operator_bug_730)
    {
      const etl::optional<int> opt = 42;

      CHECK_EQUAL(42, *opt);
    }

    //*************************************************************************
    TEST(test_arrow_operator_bug_730)
    {
      struct Object
      {
        int value;
      };

      etl::optional<Object> opt = Object{ 42 };

      CHECK_EQUAL(42, opt->value);
    }

    //*************************************************************************
    TEST(test_const_arrow_operator_bug_730)
    {
      struct Object
      {
        int value;
      };

      const etl::optional<Object> opt = Object{ 42 };

      CHECK_EQUAL(42, opt->value);
    }

#if ETL_USING_CPP14
    //*************************************************************************
    TEST(test_optional_cannot_be_constexpr_765_pod)
    {
      constexpr etl::optional<int> opt(42);

      CHECK_EQUAL(42, *opt);
    }
#endif

#if ETL_USING_CPP20 && ETL_USING_STL
    //*************************************************************************
    TEST(test_optional_cannot_be_constexpr_765_non_pod)
    {
      struct NonPod
      {
        constexpr NonPod(int v_)
          : v(v_)
        {
        }

        int v;
      };

      constexpr NonPod data(42);
      constexpr etl::optional<NonPod> opt = data;

      CHECK_EQUAL(42, (*opt).v);
    }
#endif

    //*************************************************************************
    TEST(test_optional_issue_819)
    {
      // The code below should compile without error.
      class optional_type
      {
      public:

        static etl::optional<optional_type> function()
        {
          return {};
        }
      };
    }

    //*************************************************************************
    using ItemType = etl::array<uint8_t, 2>;

    etl::optional<const ItemType> create_optional_issue_1171()
    {
      ItemType t;
      t[0] = 1;
      t[1] = 20;

      return etl::optional<const ItemType>(t);
    }

    TEST(test_optional_issue_1171)
    {
      etl::optional<const ItemType> opt1 = create_optional_issue_1171();
      CHECK_TRUE(opt1.has_value());
      CHECK_EQUAL(1, (*opt1)[0]);
      CHECK_EQUAL(20, (*opt1)[1]);

      etl::optional<const ItemType> opt2(create_optional_issue_1171());
      CHECK_TRUE(opt2.has_value());
      CHECK_EQUAL(1, (*opt2)[0]);
      CHECK_EQUAL(20, (*opt2)[1]);

      etl::optional<const ItemType> opt3;
      opt3.emplace(create_optional_issue_1171());
      CHECK_TRUE(opt3.has_value());
      CHECK_EQUAL(1, (*opt3)[0]);
      CHECK_EQUAL(20, (*opt3)[1]);
    }
  };
}
