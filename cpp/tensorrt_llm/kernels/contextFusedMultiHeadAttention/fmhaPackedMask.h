/*
 * Copyright (c) 2022-2024, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "tensorrt_llm/kernels/contextFusedMultiHeadAttention/fused_multihead_attention_common.h"
#include "tensorrt_llm/runtime/iTensor.h"
#include <cstdint>
#include <cuda_fp16.h>
#include <cuda_runtime.h>
#include <sstream>

namespace tensorrt_llm
{
namespace kernels
{

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename MaskInputDataType>
struct PackedMaskParams
{
    // The full mask input with shape [batchSize, maxQSeqLen, maxKvSeqLen].
    MaskInputDataType const* maskInput = nullptr;
    // The packed mask output with shape [numQTokens, maxKvSeqLen / 32].
    uint32_t* packedMask = nullptr;
    // The cumulative mask row offsets with shape [batchSize + 1].
    // Note the number of mask rows in each sequence needs to be padded to multiple of 128.
    int* cuMaskRows = nullptr;
    // The actual q sequence lengths (used to create cuMaskRows).
    int const* actualQSeqLens = nullptr;
    // The actual kv sequence lengths (used to construct the packed mask when full mask is not given).
    int const* actualKvSeqLens = nullptr;
    // The attention mask type (used to construct the packed mask when full mask is not given).
    ContextAttentionMaskType attentionMaskType = ContextAttentionMaskType::PADDING;
    // The batch size.
    int batchSize;
    // The maximum q sequence length.
    int maxQSeqLen;
    // The maximum kv sequence length.
    int maxKvSeqLen;
    // The sliding window size.
    int slidingWindowSize;
    // The value to identify that the position is valid.
    MaskInputDataType validPosVal = MaskInputDataType(1.0f);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename MaskInputDataType>
void invokeBuildPackedMask(PackedMaskParams<MaskInputDataType> const& params, cudaStream_t stream);

} // namespace kernels
} // namespace tensorrt_llm