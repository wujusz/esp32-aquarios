import { AxiosPromise } from 'axios';
import { AXIOS } from '../../../api/endpoints';
import { RemoteUtils } from '../utils/remoteUtils';

export interface ModuleChannel {
  id: string;
  pin: number;
}

export interface ModuleSchedule {
  startTimeHour: number;
  endTimeHour: number;
  runEvery?: number;
  offAfter?: number;
  startTimeMinute?: number;
  endTimeMinute?: number;
}

export const createAddModuleApi = (
  moduleId: string,
  moduleType: string,
  name: string,
  channels: ModuleChannel[],
  schedules: ModuleSchedule[],
  useSun?: boolean,
): AxiosPromise<void> => {
  return AXIOS.post(`${RemoteUtils.getApiBaseAddress()}createModule`, {
    id: moduleId,
    type: moduleType,
    name,
    schedules,
    useSun,
    channels,
  });
};

export const fetchModuleListApi = (): AxiosPromise<any[]> => {
  return AXIOS.get(`${RemoteUtils.getApiBaseAddress()}moduleList`);
};

export const fetchModuleApi = (moduleId: string): AxiosPromise<any> => {
  return AXIOS.get(`${RemoteUtils.getApiBaseAddress()}module?id=${moduleId}`);
};

export const updateModuleApi = (module: any): AxiosPromise<void> => {
  return AXIOS.post(`${RemoteUtils.getApiBaseAddress()}updateModule`, module);
};

