FROM nvidia/cuda:13.0.0-devel-ubuntu24.04 AS build

RUN apt update && apt install -y \
	build-essential \
	cmake \
	libcurl4-openssl-dev \
  git \
	&& rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/ggml-org/llama.cpp

RUN cp /usr/local/cuda/lib64/stubs/libcuda.so /usr/lib/x86_64-linux-gnu/libcuda.so.1 \
    && test -e /usr/lib/x86_64-linux-gnu/libcuda.so.1

RUN cmake llama.cpp -B llama.cpp/build -DGGML_CUDA=ON -DLLAMA_CURL=ON \
    -DCMAKE_CUDA_ARCHITECTURES=120 -DGGML_CUDA_FA_ALL_QUANTS=ON

# RUN cmake --build llama.cpp/build --config Release -j4 --target llama-server
RUN cmake --build llama.cpp/build --config Release -j6 \
  && rm /usr/lib/x86_64-linux-gnu/libcuda.so.1

FROM nvidia/cuda:13.0.0-runtime-ubuntu24.04
RUN apt update && apt upgrade -y && apt install -y --no-install-recommends \
        libcurl4 libgomp1 \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /llama.cpp/build/bin/ /llama.cpp/build/bin/

EXPOSE 11111

CMD ["./llama.cpp/build/bin/llama-server", "--port", "11111", "--host", "0.0.0.0", "--api-key", ""]
