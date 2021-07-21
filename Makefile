EXECUTABLE = bytesteady/bytesteady bytesteady/codec
LIBRARY = bytesteady/libbytesteady.so
OBJECT = bytesteady/city_hash.o bytesteady/fnv_hash.o  bytesteady/nll_loss.o \
	bytesteady/hinge_loss.o bytesteady/data.o bytesteady/universum.o \
	bytesteady/model.o bytesteady/train.o bytesteady/test.o \
	bytesteady/infer.o bytesteady/flags.o bytesteady/driver.o \
	bytesteady/bit_array.o bytesteady/huffman_codec.o \
	bytesteady/bytepair_codec.o bytesteady/digram_codec.o \
	bytesteady/bytehuffman_codec.o bytesteady/subsample_codec.o \
	bytesteady/codec_builder.o bytesteady/codec_coder.o \
	bytesteady/codec_flags.o bytesteady/codec_driver.o
TEST = bytesteady/nll_loss_test bytesteady/hinge_loss_test \
	bytesteady/data_test bytesteady/universum_test bytesteady/model_test \
	bytesteady/train_test bytesteady/test_test bytesteady/infer_test \
	bytesteady/driver_test bytesteady/bit_array_test \
	bytesteady/huffman_codec_test bytesteady/bytepair_codec_test \
	bytesteady/digram_codec_test bytesteady/bytehuffman_codec_test \
	bytesteady/subsample_codec_test bytesteady/codec_builder_test \
	bytesteady/codec_coder_test bytesteady/codec_driver_test
CXX ?= c++
CXXFLAGS += -std=c++17 -O3 -I.
LDFLAGS +=  -L./bytesteady -lbytesteady -pthread -lstdc++fs -lgflags -lglog \
	-lthunder_storage -lthunder_tensor -lthunder_random -lthunder_linalg \
	-lthunder_exception -lthunder_serializer
AR ?= ar
ARFLAGS +=

TEST_LDFLAGS += $(LDFLAGS) -lgtest -lgtest_main

all : $(EXECUTABLE) $(LIBRARY) $(OBJECT) $(TEST)

clean :
	-rm $(EXECUTABLE) $(LIBRARY) $(OBJECT) $(TEST)

executable : $(EXECUTABLE)

library: $(LIBRARY)

object: $(OBJECT)

test : $(TEST)

CITY_HASH_HEADER = bytesteady/city_hash.hpp
CITY_HASH_SOURCE = bytesteady/city_hash.cpp
CITY_HASH_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/city_hash.o : $(CITY_HASH_HEADER) $(CITY_HASH_SOURCE)
	$(CXX) -o $@ $(CITY_HASH_CXXFLAGS) $(CITY_HASH_SOURCE)

FNV_HASH_HEADER = bytesteady/fnv_hash.hpp
FNV_HASH_SOURCE = bytesteady/fnv_hash.cpp
FNV_HASH_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/fnv_hash.o : $(FNV_HASH_HEADER) $(FNV_HASH_SOURCE)
	$(CXX) -o $@ $(FNV_HASH_CXXFLAGS) $(FNV_HASH_SOURCE)

NLL_LOSS_HEADER = bytesteady/nll_loss.hpp bytesteady/nll_loss-inl.hpp
NLL_LOSS_SOURCE = bytesteady/nll_loss.cpp
NLL_LOSS_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/nll_loss.o : $(NLL_LOSS_HEADER) $(NLL_LOSS_SOURCE)
	$(CXX) -o $@ $(NLL_LOSS_CXXFLAGS) $(NLL_LOSS_SOURCE)

NLL_LOSS_TEST_SOURCE = bytesteady/nll_loss_test.cpp
NLL_LOSS_TEST_LIBRARY = bytesteady/libbytesteady.so
NLL_LOSS_TEST_CXXFLAGS += $(CXXFLAGS)
NLL_LOSS_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/nll_loss_test : $(NLL_LOSS_TEST_SOURCE) $(NLL_LOSS_TEST_LIBRARY)
	$(CXX) -o $@ $(NLL_LOSS_TEST_CXXFLAGS) $(NLL_LOSS_TEST_SOURCE) \
	$(NLL_LOSS_TEST_LDFLAGS)

HINGE_LOSS_HEADER = bytesteady/hinge_loss.hpp bytesteady/hinge_loss-inl.hpp
HINGE_LOSS_SOURCE = bytesteady/hinge_loss.cpp
HINGE_LOSS_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/hinge_loss.o : $(HINGE_LOSS_HEADER) $(HINGE_LOSS_SOURCE)
	$(CXX) -o $@ $(HINGE_LOSS_CXXFLAGS) $(HINGE_LOSS_SOURCE)

HINGE_LOSS_TEST_SOURCE = bytesteady/hinge_loss_test.cpp
HINGE_LOSS_TEST_LIBRARY = bytesteady/libbytesteady.so
HINGE_LOSS_TEST_CXXFLAGS += $(CXXFLAGS)
HINGE_LOSS_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/hinge_loss_test : $(HINGE_LOSS_TEST_SOURCE) \
	$(HINGE_LOSS_TEST_LIBRARY)
	$(CXX) -o $@ $(HINGE_LOSS_TEST_CXXFLAGS) $(HINGE_LOSS_TEST_SOURCE) \
	$(HINGE_LOSS_TEST_LDFLAGS)

DATA_HEADER = bytesteady/data.hpp bytesteady/data-inl.hpp
DATA_SOURCE = bytesteady/data.cpp
DATA_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/data.o : $(DATA_HEADER) $(DATA_SOURCE)
	$(CXX) -o $@ $(DATA_CXXFLAGS) $(DATA_SOURCE)

DATA_TEST_SOURCE = bytesteady/data_test.cpp
DATA_TEST_LIBRARY = bytesteady/libbytesteady.so
DATA_TEST_CXXFLAGS += $(CXXFLAGS)
DATA_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/data_test : $(DATA_TEST_SOURCE) $(DATA_TEST_LIBRARY)
	$(CXX) -o $@ $(DATA_TEST_CXXFLAGS) $(DATA_TEST_SOURCE) \
	$(DATA_TEST_LDFLAGS)

UNIVERSUM_HEADER = bytesteady/universum.hpp bytesteady/universum-inl.hpp
UNIVERSUM_SOURCE = bytesteady/universum.cpp
UNIVERSUM_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/universum.o : $(UNIVERSUM_HEADER) $(UNIVERSUM_SOURCE)
	$(CXX) -o $@ $(UNIVERSUM_CXXFLAGS) $(UNIVERSUM_SOURCE)

UNIVERSUM_TEST_SOURCE = bytesteady/universum_test.cpp
UNIVERSUM_TEST_LIBRARY = bytesteady/libbytesteady.so
UNIVERSUM_TEST_CXXFLAGS += $(CXXFLAGS)
UNIVERSUM_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/universum_test : $(UNIVERSUM_TEST_SOURCE) $(UNIVERSUM_TEST_LIBRARY)
	$(CXX) -o $@ $(UNIVERSUM_TEST_CXXFLAGS) $(UNIVERSUM_TEST_SOURCE) \
	$(UNIVERSUM_TEST_LDFLAGS)

MODEL_HEADER = bytesteady/model.hpp bytesteady/model-inl.hpp
MODEL_SOURCE = bytesteady/model.cpp
MODEL_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/model.o : $(MODEL_HEADER) $(MODEL_SOURCE)
	$(CXX) -o $@ $(MODEL_CXXFLAGS) $(MODEL_SOURCE)

MODEL_TEST_SOURCE = bytesteady/model_test.cpp
MODEL_TEST_LIBRARY = bytesteady/libbytesteady.so
MODEL_TEST_CXXFLAGS += $(CXXFLAGS)
MODEL_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/model_test : $(MODEL_TEST_SOURCE) $(MODEL_TEST_LIBRARY)
	$(CXX) -o $@ $(MODEL_TEST_CXXFLAGS) $(MODEL_TEST_SOURCE) \
	$(MODEL_TEST_LDFLAGS)

TRAIN_HEADER = bytesteady/train.hpp bytesteady/train-inl.hpp
TRAIN_SOURCE = bytesteady/train.cpp
TRAIN_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/train.o : $(TRAIN_HEADER) $(TRAIN_SOURCE)
	$(CXX) -o $@ $(TRAIN_CXXFLAGS) $(TRAIN_SOURCE)

TRAIN_TEST_SOURCE = bytesteady/train_test.cpp
TRAIN_TEST_LIBRARY = bytesteady/libbytesteady.so
TRAIN_TEST_CXXFLAGS += $(CXXFLAGS)
TRAIN_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/train_test : $(TRAIN_TEST_SOURCE) $(TRAIN_TEST_LIBRARY)
	$(CXX) -o $@ $(TRAIN_TEST_CXXFLAGS) $(TRAIN_TEST_SOURCE) \
	$(TRAIN_TEST_LDFLAGS)

TEST_HEADER = bytesteady/test.hpp bytesteady/test-inl.hpp
TEST_SOURCE = bytesteady/test.cpp
TEST_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/test.o : $(TEST_HEADER) $(TEST_SOURCE)
	$(CXX) -o $@ $(TEST_CXXFLAGS) $(TEST_SOURCE)

TEST_TEST_SOURCE = bytesteady/test_test.cpp
TEST_TEST_LIBRARY = bytesteady/libbytesteady.so
TEST_TEST_CXXFLAGS += $(CXXFLAGS)
TEST_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/test_test : $(TEST_TEST_SOURCE) $(TEST_TEST_LIBRARY)
	$(CXX) -o $@ $(TEST_TEST_CXXFLAGS) $(TEST_TEST_SOURCE) \
	$(TEST_TEST_LDFLAGS)

INFER_HEADER = bytesteady/infer.hpp bytesteady/infer-inl.hpp
INFER_SOURCE = bytesteady/infer.cpp
INFER_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/infer.o : $(INFER_HEADER) $(INFER_SOURCE)
	$(CXX) -o $@ $(INFER_CXXFLAGS) $(INFER_SOURCE)

INFER_TEST_SOURCE = bytesteady/infer_test.cpp
INFER_TEST_LIBRARY = bytesteady/libbytesteady.so
INFER_TEST_CXXFLAGS += $(CXXFLAGS)
INFER_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/infer_test : $(INFER_TEST_SOURCE) $(INFER_TEST_LIBRARY)
	$(CXX) -o $@ $(INFER_TEST_CXXFLAGS) $(INFER_TEST_SOURCE) \
	$(INFER_TEST_LDFLAGS)

FLAGS_HEADER = bytesteady/flags.hpp
FLAGS_SOURCE = bytesteady/flags.cpp
FLAGS_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/flags.o : $(FLAGS_HEADER) $(FLAGS_SOURCE)
	$(CXX) -o $@ $(FLAGS_CXXFLAGS) $(FLAGS_SOURCE)

DRIVER_HEADER = bytesteady/driver.hpp bytesteady/driver-inl.hpp
DRIVER_SOURCE = bytesteady/driver.cpp
DRIVER_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/driver.o : $(DRIVER_HEADER) $(DRIVER_SOURCE)
	$(CXX) -o $@ $(DRIVER_CXXFLAGS) $(DRIVER_SOURCE)

DRIVER_TEST_SOURCE = bytesteady/driver_test.cpp
DRIVER_TEST_OBJECT = bytesteady/driver.o bytesteady/flags.o
DRIVER_TEST_LIBRARY = bytesteady/libbytesteady.so
DRIVER_TEST_CXXFLAGS += $(CXXFLAGS)
DRIVER_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/driver_test : $(DRIVER_TEST_SOURCE) $(DRIVER_TEST_OBJECT) \
	$(DRIVER_TEST_LIBRARY)
	$(CXX) -o $@ $(DRIVER_TEST_CXXFLAGS) $(DRIVER_TEST_SOURCE) \
	$(DRIVER_TEST_OBJECT) $(DRIVER_TEST_LDFLAGS)

BYTESTEADY_SOURCE = bytesteady/bytesteady.cpp
BYTESTEADY_OBJECT = bytesteady/flags.o bytesteady/driver.o
BYTESTEADY_LIBRARY = bytesteady/libbytesteady.so
BYTESTEADY_CXXFLAGS += $(CXXFLAGS)
BYTESTEADY_LDFLAGS += $(LDFLAGS)
bytesteady/bytesteady : $(BYTESTEADY_SOURCE) $(BYTESTEADY_OBJECT) \
	$(BYTESTEADY_LIBRARY)
	$(CXX) -o $@ $(BYTESTEADY_CXXFLAGS) $(BYTESTEADY_SOURCE) \
	$(BYTESTEADY_OBJECT) $(BYTESTEADY_LDFLAGS)

BIT_ARRAY_HEADER = bytesteady/bit_array.hpp
BIT_ARRAY_SOURCE = bytesteady/bit_array.cpp
BIT_ARRAY_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/bit_array.o : $(BIT_ARRAY_HEADER) $(BIT_ARRAY_SOURCE)
	$(CXX) -o $@ $(BIT_ARRAY_CXXFLAGS) $(BIT_ARRAY_SOURCE)

BIT_ARRAY_TEST_SOURCE = bytesteady/bit_array_test.cpp
BIT_ARRAY_TEST_LIBRARY = bytesteady/libbytesteady.so
BIT_ARRAY_TEST_CXXFLAGS += $(CXXFLAGS)
BIT_ARRAY_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/bit_array_test : $(BIT_ARRAY_TEST_SOURCE) $(BIT_ARRAY_TEST_LIBRARY)
	$(CXX) -o $@ $(BIT_ARRAY_TEST_CXXFLAGS) $(BIT_ARRAY_TEST_SOURCE) \
	$(BIT_ARRAY_TEST_LDFLAGS)

HUFFMAN_CODEC_HEADER = bytesteady/huffman_codec.hpp \
	bytesteady/huffman_codec-inl.hpp
HUFFMAN_CODEC_SOURCE = bytesteady/huffman_codec.cpp
HUFFMAN_CODEC_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/huffman_codec.o : $(HUFFMAN_CODEC_HEADER) $(HUFFMAN_CODEC_SOURCE)
	$(CXX) -o $@ $(HUFFMAN_CODEC_CXXFLAGS) $(HUFFMAN_CODEC_SOURCE)

HUFFMAN_CODEC_TEST_SOURCE = bytesteady/huffman_codec_test.cpp
HUFFMAN_CODEC_TEST_LIBRARY = bytesteady/libbytesteady.so
HUFFMAN_CODEC_TEST_CXXFLAGS += $(CXXFLAGS)
HUFFMAN_CODEC_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/huffman_codec_test : $(HUFFMAN_CODEC_TEST_SOURCE) \
	$(HUFFMAN_CODEC_TEST_LIBRARY)
	$(CXX) -o $@ $(HUFFMAN_CODEC_TEST_CXXFLAGS) \
	$(HUFFMAN_CODEC_TEST_SOURCE) $(HUFFMAN_CODEC_TEST_LDFLAGS)

BYTEPAIR_CODEC_HEADER = bytesteady/bytepair_codec.hpp \
	bytesteady/bytepair_codec-inl.hpp
BYTEPAIR_CODEC_SOURCE = bytesteady/bytepair_codec.cpp
BYTEPAIR_CODEC_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/bytepair_codec.o : $(BYTEPAIR_CODEC_HEADER) $(BYTEPAIR_CODEC_SOURCE)
	$(CXX) -o $@ $(BYTEPAIR_CODEC_CXXFLAGS) $(BYTEPAIR_CODEC_SOURCE)

BYTEPAIR_CODEC_TEST_SOURCE = bytesteady/bytepair_codec_test.cpp
BYTEPAIR_CODEC_TEST_LIBRARY = bytesteady/libbytesteady.so
BYTEPAIR_CODEC_TEST_CXXFLAGS += $(CXXFLAGS)
BYTEPAIR_CODEC_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/bytepair_codec_test : $(BYTEPAIR_CODEC_TEST_SOURCE) \
	$(BYTEPAIR_CODEC_TEST_LIBRARY)
	$(CXX) -o $@ $(BYTEPAIR_CODEC_TEST_CXXFLAGS) \
	$(BYTEPAIR_CODEC_TEST_SOURCE) $(BYTEPAIR_CODEC_TEST_LDFLAGS)

DIGRAM_CODEC_HEADER = bytesteady/digram_codec.hpp \
	bytesteady/digram_codec-inl.hpp
DIGRAM_CODEC_SOURCE = bytesteady/digram_codec.cpp
DIGRAM_CODEC_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/digram_codec.o : $(DIGRAM_CODEC_HEADER) $(DIGRAM_CODEC_SOURCE)
	$(CXX) -o $@ $(DIGRAM_CODEC_CXXFLAGS) $(DIGRAM_CODEC_SOURCE)

DIGRAM_CODEC_TEST_SOURCE = bytesteady/digram_codec_test.cpp
DIGRAM_CODEC_TEST_LIBRARY = bytesteady/libbytesteady.so
DIGRAM_CODEC_TEST_CXXFLAGS += $(CXXFLAGS)
DIGRAM_CODEC_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/digram_codec_test : $(DIGRAM_CODEC_TEST_SOURCE) \
	$(DIGRAM_CODEC_TEST_LIBRARY)
	$(CXX) -o $@ $(DIGRAM_CODEC_TEST_CXXFLAGS) \
	$(DIGRAM_CODEC_TEST_SOURCE) $(DIGRAM_CODEC_TEST_LDFLAGS)

BYTEHUFFMAN_CODEC_HEADER = bytesteady/bytehuffman_codec.hpp \
	bytesteady/bytehuffman_codec-inl.hpp
BYTEHUFFMAN_CODEC_SOURCE = bytesteady/bytehuffman_codec.cpp
BYTEHUFFMAN_CODEC_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/bytehuffman_codec.o : $(BYTEHUFFMAN_CODEC_HEADER) \
	$(BYTEHUFFMAN_CODEC_SOURCE)
	$(CXX) -o $@ $(BYTEHUFFMAN_CODEC_CXXFLAGS) $(BYTEHUFFMAN_CODEC_SOURCE)

BYTEHUFFMAN_CODEC_TEST_SOURCE = bytesteady/bytehuffman_codec_test.cpp
BYTEHUFFMAN_CODEC_TEST_LIBRARY = bytesteady/libbytesteady.so
BYTEHUFFMAN_CODEC_TEST_CXXFLAGS += $(CXXFLAGS)
BYTEHUFFMAN_CODEC_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/bytehuffman_codec_test : $(BYTEHUFFMAN_CODEC_TEST_SOURCE) \
	$(BYTEHUFFMAN_CODEC_TEST_LIBRARY)
	$(CXX) -o $@ $(BYTEHUFFMAN_CODEC_TEST_CXXFLAGS) \
	$(BYTEHUFFMAN_CODEC_TEST_SOURCE) $(BYTEHUFFMAN_CODEC_TEST_LDFLAGS)

SUBSAMPLE_CODEC_HEADER = bytesteady/subsample_codec.hpp \
	bytesteady/subsample_codec-inl.hpp
SUBSAMPLE_CODEC_SOURCE = bytesteady/subsample_codec.cpp
SUBSAMPLE_CODEC_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/subsample_codec.o : $(SUBSAMPLE_CODEC_HEADER) \
	$(SUBSAMPLE_CODEC_SOURCE)
	$(CXX) -o $@ $(SUBSAMPLE_CODEC_CXXFLAGS) $(SUBSAMPLE_CODEC_SOURCE)

SUBSAMPLE_CODEC_TEST_SOURCE = bytesteady/subsample_codec_test.cpp
SUBSAMPLE_CODEC_TEST_LIBRARY = bytesteady/libbytesteady.so
SUBSAMPLE_CODEC_TEST_CXXFLAGS += $(CXXFLAGS)
SUBSAMPLE_CODEC_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/subsample_codec_test : $(SUBSAMPLE_CODEC_TEST_SOURCE) \
	$(SUBSAMPLE_CODEC_TEST_LIBRARY)
	$(CXX) -o $@ $(SUBSAMPLE_CODEC_TEST_CXXFLAGS) \
	$(SUBSAMPLE_CODEC_TEST_SOURCE) $(SUBSAMPLE_CODEC_TEST_LDFLAGS)

CODEC_BUILDER_HEADER = bytesteady/codec_builder.hpp \
	bytesteady/codec_builder-inl.hpp
CODEC_BUILDER_SOURCE = bytesteady/codec_builder.cpp
CODEC_BUILDER_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/codec_builder.o : $(CODEC_BUILDER_HEADER) $(CODEC_BUILDER_SOURCE)
	$(CXX) -o $@ $(CODEC_BUILDER_CXXFLAGS) $(CODEC_BUILDER_SOURCE)

CODEC_BUILDER_TEST_SOURCE = bytesteady/codec_builder_test.cpp
CODEC_BUILDER_TEST_LIBRARY = bytesteady/libbytesteady.so
CODEC_BUILDER_TEST_CXXFLAGS += $(CXXFLAGS)
CODEC_BUILDER_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/codec_builder_test : $(CODEC_BUILDER_TEST_SOURCE) \
	$(CODEC_BUILDER_TEST_LIBRARY)
	$(CXX) -o $@ $(CODEC_BUILDER_TEST_CXXFLAGS) \
	$(CODEC_BUILDER_TEST_SOURCE) $(CODEC_BUILDER_TEST_LDFLAGS)

CODEC_CODER_HEADER = bytesteady/codec_coder.hpp bytesteady/codec_coder-inl.hpp
CODEC_CODER_SOURCE = bytesteady/codec_coder.cpp
CODEC_CODER_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/codec_coder.o : $(CODEC_CODER_HEADER) $(CODEC_CODER_SOURCE)
	$(CXX) -o $@ $(CODEC_CODER_CXXFLAGS) $(CODEC_CODER_SOURCE)

CODEC_FLAGS_HEADER = bytesteady/codec_flags.hpp
CODEC_FLAGS_SOURCE = bytesteady/codec_flags.cpp
CODEC_FLAGS_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/codec_flags.o : $(CODEC_FLAGS_HEADER) $(CODEC_FLAGS_SOURCE)
	$(CXX) -o $@ $(CODEC_FLAGS_CXXFLAGS) $(CODEC_FLAGS_SOURCE)

CODEC_CODER_TEST_SOURCE = bytesteady/codec_coder_test.cpp
CODEC_CODER_TEST_LIBRARY = bytesteady/libbytesteady.so
CODEC_CODER_TEST_CXXFLAGS += $(CXXFLAGS)
CODEC_CODER_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/codec_coder_test : $(CODEC_CODER_TEST_SOURCE) \
	$(CODEC_CODER_TEST_LIBRARY)
	$(CXX) -o $@ $(CODEC_CODER_TEST_CXXFLAGS) \
	$(CODEC_CODER_TEST_SOURCE) $(CODEC_CODER_TEST_LDFLAGS)

CODEC_DRIVER_HEADER = bytesteady/codec_driver.hpp \
	bytesteady/codec_driver-inl.hpp
CODEC_DRIVER_SOURCE = bytesteady/codec_driver.cpp
CODEC_DRIVER_CXXFLAGS += $(CXXFLAGS) -c -fPIC
bytesteady/codec_driver.o : $(CODEC_DRIVER_HEADER) $(CODEC_DRIVER_SOURCE)
	$(CXX) -o $@ $(CODEC_DRIVER_CXXFLAGS) $(CODEC_DRIVER_SOURCE)

CODEC_DRIVER_TEST_SOURCE = bytesteady/codec_driver_test.cpp
CODEC_DRIVER_TEST_OBJECT = bytesteady/codec_driver.o bytesteady/codec_flags.o
CODEC_DRIVER_TEST_LIBRARY = bytesteady/libbytesteady.so
CODEC_DRIVER_TEST_CXXFLAGS += $(CXXFLAGS)
CODEC_DRIVER_TEST_LDFLAGS += $(TEST_LDFLAGS)
bytesteady/codec_driver_test : $(CODEC_DRIVER_TEST_SOURCE) \
	$(CODEC_DRIVER_TEST_OBJECT) $(CODEC_DRIVER_TEST_LIBRARY)
	$(CXX) -o $@ $(CODEC_DRIVER_TEST_CXXFLAGS) $(CODEC_DRIVER_TEST_SOURCE) \
	$(CODEC_DRIVER_TEST_OBJECT) $(CODEC_DRIVER_TEST_LDFLAGS)

CODEC_SOURCE = bytesteady/codec.cpp
CODEC_OBJECT = bytesteady/codec_flags.o bytesteady/codec_driver.o
CODEC_LIBRARY = bytesteady/libbytesteady.so
CODEC_CXXFLAGS += $(CXXFLAGS)
CODEC_LDFLAGS += $(LDFLAGS)
bytesteady/codec : $(CODEC_SOURCE) $(CODEC_OBJECT) $(CODEC_LIBRARY)
	$(CXX) -o $@ $(CODEC_CXXFLAGS) $(CODEC_SOURCE) $(CODEC_OBJECT) \
	$(CODEC_LDFLAGS)

LIBBYTESTEADY_OBJECT = bytesteady/city_hash.o bytesteady/fnv_hash.o \
	bytesteady/nll_loss.o bytesteady/hinge_loss.o bytesteady/data.o \
	bytesteady/universum.o bytesteady/model.o bytesteady/train.o \
	bytesteady/test.o bytesteady/infer.o bytesteady/bit_array.o \
	bytesteady/huffman_codec.o bytesteady/bytepair_codec.o \
	bytesteady/digram_codec.o bytesteady/bytehuffman_codec.o \
	bytesteady/subsample_codec.o bytesteady/codec_builder.o \
	bytesteady/codec_coder.o
LIBBYTESTEADY_LDFLAGS += -shared
bytesteady/libbytesteady.so : $(LIBBYTESTEADY_OBJECT)
	$(CXX) -o $@ $(LIBBYTESTEADY_OBJECT) $(LIBBYTESTEADY_LDFLAGS)
