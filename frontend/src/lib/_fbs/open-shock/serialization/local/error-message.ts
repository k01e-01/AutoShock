// automatically generated by the FlatBuffers compiler, do not modify

/* eslint-disable @typescript-eslint/no-unused-vars, @typescript-eslint/no-explicit-any, @typescript-eslint/no-non-null-assertion */

import * as flatbuffers from 'flatbuffers';

export class ErrorMessage {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):ErrorMessage {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsErrorMessage(bb:flatbuffers.ByteBuffer, obj?:ErrorMessage):ErrorMessage {
  return (obj || new ErrorMessage()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsErrorMessage(bb:flatbuffers.ByteBuffer, obj?:ErrorMessage):ErrorMessage {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new ErrorMessage()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

message():string|null
message(optionalEncoding:flatbuffers.Encoding):string|Uint8Array|null
message(optionalEncoding?:any):string|Uint8Array|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? this.bb!.__string(this.bb_pos + offset, optionalEncoding) : null;
}

static startErrorMessage(builder:flatbuffers.Builder) {
  builder.startObject(1);
}

static addMessage(builder:flatbuffers.Builder, messageOffset:flatbuffers.Offset) {
  builder.addFieldOffset(0, messageOffset, 0);
}

static endErrorMessage(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static createErrorMessage(builder:flatbuffers.Builder, messageOffset:flatbuffers.Offset):flatbuffers.Offset {
  ErrorMessage.startErrorMessage(builder);
  ErrorMessage.addMessage(builder, messageOffset);
  return ErrorMessage.endErrorMessage(builder);
}
}
