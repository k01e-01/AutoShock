// automatically generated by the FlatBuffers compiler, do not modify

/* eslint-disable @typescript-eslint/no-unused-vars, @typescript-eslint/no-explicit-any, @typescript-eslint/no-non-null-assertion */

import * as flatbuffers from 'flatbuffers';

export class ExtensionConfig {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):ExtensionConfig {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsExtensionConfig(bb:flatbuffers.ByteBuffer, obj?:ExtensionConfig):ExtensionConfig {
  return (obj || new ExtensionConfig()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsExtensionConfig(bb:flatbuffers.ByteBuffer, obj?:ExtensionConfig):ExtensionConfig {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new ExtensionConfig()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

/**
 * Plaintext json configuration for all extensions
 */
ptExtensionsConfig():string|null
ptExtensionsConfig(optionalEncoding:flatbuffers.Encoding):string|Uint8Array|null
ptExtensionsConfig(optionalEncoding?:any):string|Uint8Array|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? this.bb!.__string(this.bb_pos + offset, optionalEncoding) : null;
}

static startExtensionConfig(builder:flatbuffers.Builder) {
  builder.startObject(1);
}

static addPtExtensionsConfig(builder:flatbuffers.Builder, ptExtensionsConfigOffset:flatbuffers.Offset) {
  builder.addFieldOffset(0, ptExtensionsConfigOffset, 0);
}

static endExtensionConfig(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static createExtensionConfig(builder:flatbuffers.Builder, ptExtensionsConfigOffset:flatbuffers.Offset):flatbuffers.Offset {
  ExtensionConfig.startExtensionConfig(builder);
  ExtensionConfig.addPtExtensionsConfig(builder, ptExtensionsConfigOffset);
  return ExtensionConfig.endExtensionConfig(builder);
}
}
