# DOT2BLIF


## Compile the DOT2BLIF code from the source repository, where this README lies:

```bash
mkdir bin
make
```

## Install dependencies

In order to run the Buffers command you will need to install graphviz, graphviz-dev and pkg-config
(if not already installed), which you can do with the following commands:

```bash
sudo apt install graphviz
sudo apt install graphviz-dev
sudo apt install pkg-config
```

## Run buffer opt

```bash
bin/convert blif -filename=name
```

Here, period indicates target CP (ns).

For example:
```bash
bin/convert blif -filename=examples/loop_array
```

To see all command options and parameters, run:
```bash
bin/convert blif -help
```
