# nRF Connect SDK Playground

[![Current Version](https://img.shields.io/github/tag/makerdiary/ncs-playground.svg)](https://github.com/makerdiary/ncs-playground/tags)
[![Samples](https://github.com/makerdiary/ncs-playground/actions/workflows/samples.yml/badge.svg?branch=main)](https://github.com/makerdiary/ncs-playground/actions/workflows/samples.yml?query=branch%3Amain)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?color=informational)](/.github/CONTRIBUTING.md)

nRF Connect SDK Playground contains a range of applications, samples, and libraries for Makerdiary's development hardware.

## Supported Hardware

| Hardware platform      | Build target          |
|------------------------|-----------------------|
| [nRF52840 Connect Kit] | `connectkit_nrf52840` |


## Getting Started

Before getting started, make sure you have a proper nRF Connect SDK development environment. Follow the official [Getting started guide].

### Initialization
The first step is to initialize the workspace folder (`my-workspace`) where the `ncs-playground` and all nRF Connect SDK modules will be cloned. Run the following command:

``` sh
# initialize my-workspace for the ncs-playground (main branch)
west init -m https://github.com/makerdiary/ncs-playground --mr main my-workspace
# update nRF Connect SDK modules
cd my-workspace
west update
```

### Building and running

After completing the environment setup, run the following command to build the sample (for example, [Blinky]):

``` sh
west build -p always -b $BOARD samples/blinky
```

where `$BOARD` is the target board and `-p always` option forces a pristine build.

After running the `west build` command, the build files can be found in `build/zephyr`. If the board supports UF2 Bootloader, you will find the firmware with the name `zephyr.uf2`.

## Community Support

Community support is provided via [GitHub Discussions][discussions]. You can also reach us on
[Makerdiary Community][community].

We would love to have more developers contribute to this project! If you're passionate about making this project better, see our [Contributing Guidelines][contributing] for more information.

## License

Copyright (c) 2016-2023 Makerdiary. See [LICENSE](./LICENSE) for further details.

[nRF52840 Connect Kit]: https://wiki.makerdiary.com/nrf52840-connectkit
[Blinky]: /samples/blinky
[Getting started guide]: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html
[discussions]: https://github.com/makerdiary/ncs-playground/discussions
[community]: https://community.makerdiary.com
[contributing]: /.github/CONTRIBUTING.md
