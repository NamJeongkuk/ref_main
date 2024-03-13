/*!
 * @file
 * @brief Run a test with a matrix of parameters
 *
 * Usage:
 * #define PERSONALITIES(ENTRY) \
 *    ENTRY(Blah) \
 *
 * Then in your test group:
 * void setup(personality)
 *
 * And in your tests:
 * MATRIX_TEST(testGroup, testName, PERSONALITIES)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#pragma once

#include "CppUTest/SimpleString.h"
#include "CppUTest/TestHarness.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupport.h"

template <unsigned N>
struct String
{
   char c[N];
};

template <unsigned... Len>
constexpr auto concat(const char (&...strings)[Len])
{
   constexpr unsigned N = (... + Len) - sizeof...(Len);
   String<N + 1> result = {};
   result.c[N] = '\0';

   char *dst = result.c;
   for(const char *src : { strings... })
   {
      for(; *src != '\0'; src++, dst++)
      {
         *dst = *src;
      }
   }
   return result;
}

template <typename T>
struct TestShell : public UtestShell
{
   TestShell(const char *fileName, int lineNumber)
      : UtestShell(T::groupName, T::name, fileName, lineNumber)
   {
      TestRegistry::getCurrentRegistry()->addTest(this);
   }

   Utest *createTest() override
   {
      return new T();
   }
};

template <typename T, typename U>
struct TestRunner : public T
{
   static constexpr auto _name = concat(T::name, " (", U::name, ")");
   static constexpr auto name = _name.c;

   void setup()
   {
      T::setup(U::value);
   }
};

#define MATRIX_TEST_INSTALLER(enum)         \
   struct Test_##enum                       \
   {                                        \
      static constexpr auto value = enum;   \
      static constexpr char name[] = #enum; \
   };                                       \
                                            \
   static TestShell<TestRunner<BaseTest, Test_##enum>> TestShell_##enum(__FILE__, __LINE__);

#define MATRIX_TEST(testGroup, testName, entries)                    \
   namespace testGroup##_##testName                                  \
   {                                                                 \
      struct BaseTest : public TEST_GROUP_##CppUTestGroup##testGroup \
      {                                                              \
         static constexpr char groupName[] = #testGroup;             \
         static constexpr char name[] = #testName;                   \
         void testBody() override;                                   \
      };                                                             \
                                                                     \
      entries(MATRIX_TEST_INSTALLER)                                 \
   }                                                                 \
                                                                     \
   void testGroup##_##testName::BaseTest::testBody()
