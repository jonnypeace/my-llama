FROM nvidia/cuda:13.0.0-devel-ubuntu24.04

RUN apt update && apt install -y  g++ freeglut3-dev \
	build-essential libx11-dev libxmu-dev libxi-dev \
	libglu1-mesa-dev libfreeimage-dev libglfw3-dev \
	wget htop btop nvtop glances git \
	pciutils build-essential cmake \
	curl libcurl4-openssl-dev python3 python3-pip \
	nvidia-driver-580-open \
	&& rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/ggml-org/llama.cpp

RUN cmake llama.cpp -B llama.cpp/build -DGGML_CUDA=ON -DLLAMA_CURL=ON \
    -DCMAKE_CUDA_ARCHITECTURES=120

# RUN cmake --build llama.cpp/build --config Release -j4 --target llama-server
RUN cmake --build llama.cpp/build --config Release -j6

EXPOSE 11111

CMD ["./llama.cpp/build/bin/llama-server", "--port", "11111", "--host", "0.0.0.0"]
