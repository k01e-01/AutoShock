import type { WiFiNetwork } from './WiFiNetwork';

export type WiFiState = {
  initialized: boolean;
  connected: string | null;
  scanning: boolean;
  networks: Map<string, WiFiNetwork>;
};
