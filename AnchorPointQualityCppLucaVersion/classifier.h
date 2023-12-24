#ifndef CLASSIFIER_H
#define CLASSIFIER_H

template<typename VoxelGridDecorator>
class Classifier
{
	public:
		Classifier(VoxelGridDecorator & _VGD)
		:	VGD(_VGD)
		{
		}
		
		int test()
		{
			return VGD.get(0,0,0);
		}
	private:
		VoxelGridDecorator & VGD;
};

#endif