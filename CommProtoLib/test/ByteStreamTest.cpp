#include <gtest/gtest.h>

#include <commproto/parser/ByteStream.h>
#include "commproto/utils/Math.h"

namespace commproto
{
	namespace parser
	{
		namespace test
		{
			template <typename T>
			class ByteStreamTest : public testing::Test
			{
			};


			TYPED_TEST_SUITE_P(ByteStreamTest);

			TYPED_TEST_P(ByteStreamTest,CanReadAndWrite)
			{
				const TypeParam input = 42;
				TypeParam output;

				ByteStream writer;
				writer.write(input);
				ByteStream reader(writer.getStream());

				bool readResult = reader.read(output);
				ASSERT_TRUE(readResult);
				ASSERT_EQ(input, output);

			}

			REGISTER_TYPED_TEST_SUITE_P(ByteStreamTest,CanReadAndWrite);

            using WriteableTypes = ::testing::Types<uint8_t, int8_t,uint16_t,int16_t, uint32_t, uint64_t,int64_t,float,double,bool>;
			INSTANTIATE_TYPED_TEST_SUITE_P(ByteStreamTestSuite, ByteStreamTest, WriteableTypes);


			TEST(ByteStream,CanReadAndWriteString)
			{
				ByteStream writer;
				std::string input = "test";
				std::string output;


				writer.write(input);

				ByteStream reader(writer.getStream());

				bool res = reader.read(output);

				ASSERT_TRUE(res);
				ASSERT_EQ(input, output);
			}

			TEST(ByteStream, CanReadAndWriteVector)
			{
				ByteStream writer;
				std::vector<uint32_t> input;
				std::vector<uint32_t> output;

				for(uint32_t index = 0;index<42;++index)
				{
					input.push_back(42 - index);
				}

				writer.write(input);

				ByteStream reader(writer.getStream());

				bool res = reader.read(output);

				ASSERT_TRUE(res);
				ASSERT_EQ(input, output);
			}

			TEST(ByteStream, CanReadAndWriteMessage)
			{
				ByteStream writer;
				Message input;
				Message output;

				for (uint32_t index = 0; index<42; ++index)
				{
					input.push_back(42 - index);
				}

				writer.write(input);

				ByteStream reader(writer.getStream());

				bool res = reader.read(output);

				ASSERT_TRUE(res);
				ASSERT_EQ(input, output);
			}


			TEST(TestNearestFunction,CanGetAccurateNumbers)
			{
				ASSERT_NEAR(30.f,math::getNearest(0.f,100.f,0.37,10.f),0.01f);
				ASSERT_NEAR(.5f,math::getNearest(0.f,10.f,0.056f,.5f),0.01f);
				ASSERT_NEAR(.0f,math::getNearest(0.f,10.f,0.03f,.5f),0.01f);
				ASSERT_NEAR(0.f,math::getNearest(0.f,10.f,0.047f,5.f),0.01f);
				ASSERT_NEAR(10.f,math::getNearest(0.f,10.f,0.99f,5.f),0.01f);
				ASSERT_NEAR(0.f,math::getNearest(0.f,10.f,0.01f,5.f),0.01f);
			}
		}
	}
}
