.PHONY: all

all: deviceCapability
	@:

deviceCapability: deviceCapability.cu
	nvcc deviceCapability.cu -o deviceCapability

clean:
	$(RM) deviceCapability
