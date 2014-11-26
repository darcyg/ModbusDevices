#include "stdafx.h"
#include "CppUnitTest.h"
#include "Value.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft{
  namespace VisualStudio {
    namespace CppUnitTestFramework
    {
      template<>
      static std::wstring ToString<Value>(const Value& t)
      {
        return std::wstring();
      }
    }
  }
}

namespace ModbusDevicesTest
{		
	TEST_CLASS(UnitTestValue)
	{
	public:
		
		TEST_METHOD(TestParse)
		{
      Value val;
      Assert::IsFalse(Value::parse(".}", &val));
      Assert::IsFalse(Value::parse("{.", &val));
      Assert::IsFalse(Value::parse("{}", &val));
      Assert::IsTrue(Value::parse("{.}", nullptr));
      Assert::AreEqual((int)val.type, (int)Value::TYPE_U32);
      Assert::AreEqual((int)val.v_u32, 0);
      Assert::AreEqual((int)val.divisor, 1);
      Assert::AreEqual((int)val.multiplier, 1);
      Assert::AreEqual((int)val.fractional, 0);

      Assert::IsTrue(Value::parse("{.4s}", &val));
      Assert::AreEqual(val, Value(Value::TYPE_S32, 4));

      Assert::IsTrue(Value::parse("{.5f*10}", &val));
      Assert::AreEqual(val, Value(Value::TYPE_FLOAT, 0, 10));

      Assert::IsTrue(Value::parse("{.5f:5*10}", &val));
      Assert::AreEqual(val, Value(Value::TYPE_FLOAT, 0, 10, 5));

      Assert::IsTrue(Value::parse("{45.5s45:5*10}", &val));
      Assert::AreEqual(val, Value(Value::TYPE_S32, 5, 10, 5));
		}

    TEST_METHOD(TestConvert)
    {
      Value val, val2;
      Assert::IsTrue(Value::parse("{.2}", &val));
      val = 456;
      Assert::AreEqual(val.v_u32, (unsigned int)45600);
      val = 1.23;
      Assert::AreEqual(val.v_u32, (unsigned int)123);
      val = 4.567;
      Assert::AreEqual(val.v_u32, (unsigned int)457);
      
      Assert::IsTrue(Value::parse("{f}", &val));
      val = 456;
      Assert::AreEqual(val.v_float, 456.0f);
      val = 1.23;
      Assert::AreEqual(val.v_float, 1.23f);
      val = 4.567;
      Assert::AreEqual(val.v_float, 4.567f);

      Assert::IsTrue(Value::parse("{.3}", &val));
      Assert::IsTrue(Value::parse("{f}", &val2));
      val2.v_float = 123.456f;
      val = val2;
      Assert::AreEqual(val.v_u32, (unsigned int)123456);
      val.v_u32 = 456789;
      val2 = val;
      Assert::AreEqual(val2.v_float, 456.789f);

      Assert::IsTrue(Value::parse("{.3}", &val));
      Assert::IsTrue(Value::parse("{.1}", &val2));
      val2.v_u32 = 123;
      val = val2;
      Assert::AreEqual(val.v_u32, (unsigned int)12300);
      val.v_u32 = 456789;
      val2 = val;
      Assert::AreEqual(val2.v_u32, (unsigned int)4567);
    }
	};
}