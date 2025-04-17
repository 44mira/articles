# use std::cell::RefCell;
# use std::rc::Rc;
#[derive(Debug)]
struct Node {
    name: String,
    neighbors: Vec<Rc<RefCell<Node>>>,
    neighbor_count: u8,
}

impl Node {
    fn new(name: &str, neighbors: Vec<Rc<RefCell<Node>>>) -> Rc<RefCell<Node>> {
        let node = Rc::new(RefCell::new(Node {
            name: String::from(name),
            neighbors: vec![],
            neighbor_count: 0,
        }));

        for neighbor in neighbors {
            node.borrow_mut().connect(neighbor);
        }

        node
    }

    fn connect(&mut self, other: Rc<RefCell<Node>>) -> &mut Self {
        self.neighbors.push(Rc::clone(&other));
        other.borrow_mut().neighbor_count += 1;
        self.neighbor_count += 1;

        self
    }
}
