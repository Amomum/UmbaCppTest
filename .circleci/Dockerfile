FROM ubuntu:19.10

# Install java, gcc & sanitizer
RUN apt-get update && apt-get install -y default-jdk \
    gcc g++ \
    libasan5 \
    gcc-arm-none-eabi \
    wget

# Set the working directory to /app
WORKDIR /app

# download and unzip eclipse
# that's not the BEST practice but I don't want to add this 200+ MB file in my repo; eclipse from official apt repository didn't work well
RUN wget --progress=bar:force:noscroll https://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/oxygen/3/eclipse-cpp-oxygen-3-linux-gtk-x86_64.tar.gz -O eclipse.tar.gz && \
    tar -xzf eclipse.tar.gz -C /app
