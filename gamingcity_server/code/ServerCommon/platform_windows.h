#pragma once

// linux todo
#ifdef PLATFORM_WINDOWS
/// ���CPU�ĺ���  
int get_processor_number();

/// �õ�����cpuռ��
int get_cpu_usage(int pid);

/// �õ���ǰ�ڴ�ʹ����Ϣ
void get_memory_info(int& workingSetSize, int& peakWorkingSetSize, int& pagefileUsage, int& peakPagefileUsage);
#endif
