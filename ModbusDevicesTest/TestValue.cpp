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
	};
}