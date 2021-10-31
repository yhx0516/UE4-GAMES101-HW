#include "BVHAccelTree.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UBVHAccelTree::UBVHAccelTree() {}

void UBVHAccelTree::Init(TArray<TScriptInterface<IHw7_Object>> p, int _maxPrimsInNode /*= 1*/,
	SplitMethod _splitMethod /*= SplitMethod::NAIVE*/, bool _bDraw /*= false*/)
{
	primitives = MoveTemp(p);
	maxPrimsInNode = _maxPrimsInNode;
	splitMethod = splitMethod;
	bDraw = _bDraw;

	if (primitives.Num() == 0)
		return;

	// 构造根节点
	root = recursiveBuild(primitives);
}



// 构造子树 ，递归
UBVHTreeNode* UBVHAccelTree::recursiveBuild(TArray<TScriptInterface<IHw7_Object>> objects)
{

	if (objects.Num() == 0)
		return nullptr;

	UBVHTreeNode* node = NewObject<UBVHTreeNode>();

	// Compute bounds of all primitives in BVH node
	Bounds3 bounds;
	for (int i = 0; i < objects.Num(); ++i)
		bounds.Union(objects[i]->getBounds());

	if (objects.Num() == 1) {
		// Create leaf _BVHBuildNode_
		node->bounds = objects[0]->getBounds();
		node->object = objects[0];
		node->left = nullptr;
		node->right = nullptr;
		node->area = objects[0]->getArea();
		if (bDraw) {
			bounds.Draw(GetOuter()->GetWorld(), FLinearColor::Yellow, 1,2.0f);
		}
		return node;
	}
	else if (objects.Num() == 2) {
		node->left = recursiveBuild(TArray<TScriptInterface<IHw7_Object>>{objects[0]});
		node->right = recursiveBuild(TArray<TScriptInterface<IHw7_Object>>{objects[1]});
		node->bounds.Union(node->left->bounds);
		node->bounds.Union(node->right->bounds);
		node->area = node->left->area + node->right->area;
		return node;
	}
	else {
		Bounds3 centroidBounds;
		for (int i = 0; i < objects.Num(); ++i)
			centroidBounds.Union(objects[i]->getBounds().Centroid());

		// 获取最长轴
		int dim = centroidBounds.maxExtent();

		// 按最长轴坐标 排序
		objects.Sort(
			[dim](const TScriptInterface<IHw7_Object>& f1, const TScriptInterface<IHw7_Object>& f2) {
				return f1->getBounds().Centroid()[dim] < f2->getBounds().Centroid()[dim];
			});
		// 分成两部分
		TArray<TScriptInterface<IHw7_Object>> leftshapes, rightshapes;
		for (int i = 0; i < objects.Num(); i++) {
			if (i < objects.Num() / 2)
				leftshapes.Add(objects[i]);
			else
				rightshapes.Add(objects[i]);
		}

		ensure(objects.Num() == (leftshapes.Num() + rightshapes.Num()));

		node->left = recursiveBuild(leftshapes);
		node->right = recursiveBuild(rightshapes);
		node->bounds.Union(node->left->bounds);
		node->bounds.Union(node->right->bounds);
		node->area = node->left->area + node->right->area;
	}


	return node;
}

//【多线程】 光线与空间相交
Intersection UBVHAccelTree::Intersect(const Ray& ray) const
{
	Intersection isect;
	if (!root)
		return isect;
	isect = getIntersection(root, ray);
	return isect;
}
//【多线程】
Intersection UBVHAccelTree::getIntersection(UBVHTreeNode* node, const Ray& ray) const
{
	// TODO Traverse the BVH to find intersection
	Intersection isect;
	if (node == nullptr)
		return isect;

	FVector dirisNeg = FVector::ZeroVector; //轴正负
	dirisNeg.X = ray.direction.X > 0 ? 1 : -1;
	dirisNeg.Y = ray.direction.Y > 0 ? 1 : -1;
	dirisNeg.Z = ray.direction.Z > 0 ? 1 : -1;
	if (false == node->bounds.IntersectP(ray, ray.direction_inv, dirisNeg)) //判断是否在包围盒
		return  isect;

	if (node->left == nullptr && node->right == nullptr) { //直到叶子节点
		isect = node->object->getIntersection(ray);	
		return isect;
	}

	Intersection leftInter = getIntersection(node->left, ray); // 不到叶子，就一直递归下去
	Intersection rightInter = getIntersection(node->right, ray);

	return leftInter.distance < rightInter.distance ? leftInter : rightInter; //选比较近的
}
//【多线程】采样
void UBVHAccelTree::getSample(UBVHTreeNode* node, float p, Intersection& pos, float& pdf)
{
	if (node->left == nullptr || node->right == nullptr) {
		node->object->Sample(pos, pdf);
		pdf *= node->area;
		return;
	}
	if (p < node->left->area) getSample(node->left, p, pos, pdf);
	else getSample(node->right, p - node->left->area, pos, pdf);
}
//【多线程】采样
void UBVHAccelTree::Sample(Intersection& pos, float& pdf)
{
	float p = std::sqrt(UKismetMathLibrary::RandomFloat()) * root->area;
	getSample(root, p, pos, pdf);
	pdf /= root->area;
}