#m7 #rendering
This is complicated because the data has to be split in so many different ways. 

Splits:
Viewports - Each viewport does culling individually
Materials - Each material also (because of different render calls)

```
threadcount = viewports * pipelines
threads(threadcount)
func(thread_index)
{
	int viewport_index = thread_index % viewports
	int pipeline_index = thread_index % pipelines
	
	auto frustum = frustums[viewport_index]
	// Cull using frustum
	auto[] pipeline_instances = instances[pipeline_index]
	
	for (auto instance_index : pipeline_instances)
		if (frustum.check(instance_index))
			output[viewport_index][pipeline_index].add(instance_index)
}
```

From then on, the culled instances live on the GPU only!

