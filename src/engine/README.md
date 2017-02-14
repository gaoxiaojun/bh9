# Engine
Engine Moudle include core function for hal9000 trading framework

引擎的目的是提供一个虚拟的时钟和虚拟定时器，能同时在实时和仿真的状态下正确工作。
这个虚拟定时器作用：
1、用于生成Bar
2、用于处理一些定时任务（例如开始、结束交易时间设定等等）

至于对Event按照时间顺序排序之类的功能其实不是引擎必备的功能。

思考：
  pipe里包含多个queue，bus里包含多个pipe的架构其实是不必要的，
  单一的优先级队列就足够了。

# MarketData
数据在内存的安排问题比较头大
