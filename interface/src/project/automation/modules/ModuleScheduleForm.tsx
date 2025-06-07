import { FC, useState } from 'react';
import { DateRange } from 'rsuite/DateRangePicker';
import { ValidateFieldsError } from 'async-validator';
import { updateValue } from '../../utils';
import { channelStateContent } from '../channels/rest/channelStateContent';

export interface ModuleScheduleState {
  schedule: any;
  enabled: boolean;
  enableTimeSpan: boolean;
  randomize: boolean;
  enableMinimumRunTime: boolean;
  enableDateRange: boolean;
  activeOutsideDateRange: boolean;
  brightness?: number;
}

const defaultScheduleState = (): ModuleScheduleState => ({
  schedule: {
    runEvery: 15,
    offAfter: 3,
    startTimeHour: 8,
    startTimeMinute: 0,
    endTimeHour: 16,
    endTimeMinute: 0,
    hotTimeHour: 0,
    overrideTime: 15,
    weekDays: [0,1,2,3,4,5,6],
    isOverride: false,
  },
  enabled: true,
  enableTimeSpan: false,
  randomize: false,
  enableMinimumRunTime: false,
  enableDateRange: false,
  activeOutsideDateRange: false,
  brightness: 0,
});

interface Props {
  value: ModuleScheduleState;
  onChange: (val: ModuleScheduleState) => void;
}

const ModuleScheduleForm: FC<Props> = ({ value, onChange }) => {
  const [state, setState] = useState<ModuleScheduleState>(value);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [range, setRange] = useState<DateRange>([new Date(), new Date()]);
  const updateFormValue = updateValue(setState);

  const content = channelStateContent(
    state as any,
    async () => {},
    undefined,
    (d) => { if (d) { setState(d as ModuleScheduleState); onChange(d as ModuleScheduleState); } },
    () => {},
    0,
    0,
    setFieldErrors,
    () => {},
    fieldErrors,
    updateFormValue,
    range,
    setRange,
    undefined,
    false
  );

  return (<div>{content()}</div>);
};

export { ModuleScheduleForm, defaultScheduleState };
