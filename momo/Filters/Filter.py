from abc import abstractmethod, ABC


class Filter(ABC):
    @abstractmethod
    def validate(self, Path_list):
        pass
