// automatically generated by the FlatBuffers compiler, do not modify

/* eslint-disable @typescript-eslint/no-unused-vars, @typescript-eslint/no-explicit-any, @typescript-eslint/no-non-null-assertion */

import * as flatbuffers from 'flatbuffers';

import { AccountLinkResultCode } from '../../../open-shock/serialization/local/account-link-result-code';


export class AccountLinkCommandResult {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):AccountLinkCommandResult {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

result():AccountLinkResultCode {
  return this.bb!.readUint8(this.bb_pos);
}

static sizeOf():number {
  return 1;
}

static createAccountLinkCommandResult(builder:flatbuffers.Builder, result: AccountLinkResultCode):flatbuffers.Offset {
  builder.prep(1, 1);
  builder.writeInt8(result);
  return builder.offset();
}

}
