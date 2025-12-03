from platformio.public import TestRunnerBase
class CustomTestRunner(TestRunnerBase):
    EXTRA_LIB_DEPS = []
    def configure_build_env(self, env):
        env.Append(LIBPATH=[], LIBS=[])  # No Unity